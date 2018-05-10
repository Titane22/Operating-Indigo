// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "OperationIndigoPlayerController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "TacticalCamera.h"
#include "PlayerAIController.h"
#include "EnemyAIController.h"
#include "OperationIndigoCharacter.h"
#include "BattleHUD.h"
#include "Tile.h"
#include "Kismet/GameplayStatics.h"

AOperationIndigoPlayerController::AOperationIndigoPlayerController()
{
	bShowMouseCursor = true;
}



void AOperationIndigoPlayerController::BeginPlay()
{
	Super::BeginPlay();
	//ActivateBattlePhase();
	SetInputMode(FInputModeGameAndUI());
}

void AOperationIndigoPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	// If it is a Battle Phase
	if (bBattlePhase)
	{
		if (!bActivatedUnit)
		{
			// Find Activated Unit in UnitsInBattlePhase
			for (auto Unit : UnitsInBattlePhase)
			{
				Unit->StartGauge();
				if (Unit->isActivated())
				{
					SelectCharacter(Unit);
					bActivatedUnit = true;
					bStopGauge = true;
					AIController = Cast<AEnemyAIController>(Unit->GetController());
					if (AIController)
					{
						AIController->bStartTurn = true;
					}
					// Move Camera to Activated character
					auto PlayerCamera = GetPawn();
					auto MoveToLocation = Unit->GetActorLocation();
					MoveToLocation.Z = PlayerCamera->GetActorLocation().Z;
					PlayerCamera->SetActorLocation(MoveToLocation);
					break;
				}
			}
		}
		// If any of UnitsInBattlePhase is activated
		else
		{
			// Stop the Gauge.
			if (bStopGauge)
			{
				for (auto Unit : UnitsInBattlePhase)
				{
					Unit->StopGauge();
				}
				bStopGauge = false;
			}
			// If ActivatedCharacter is deactivated, Find again.
			if (!SelectedCharacter->isActivated() && bActivatedUnit)
			{
				InitSelection();
				bActivatedUnit = false;
				if (AIController)
				{
					AIController = nullptr;
				}
			}
		}//bActivatedUnit
		
		// Grid Tracing control by Mouse
		GridTracingControl();
		ShowStateOfTile();
	}
}

void AOperationIndigoPlayerController::GridTracingControl()
{
	FHitResult TraceHit;

	FVector WorldLocation;
	FVector WorldDirection;

	FVector StartMousePos;
	FVector EndMousePos;
	DeprojectMousePositionToWorld(OUT WorldLocation, OUT WorldDirection);
	StartMousePos = WorldLocation;
	EndMousePos = WorldLocation + WorldDirection*5000.f;
	FCollisionQueryParams CollisionParams;
	
	GetWorld()->LineTraceSingleByChannel(
		TraceHit,
		StartMousePos,
		EndMousePos,
		ECC_GameTraceChannel2
	);

	auto TraceActor = Cast<ATile>(TraceHit.GetActor());
	// TODO : Change MoveRange System
	EstimateTileState(TraceActor);
}

void AOperationIndigoPlayerController::EstimateTileState(ATile * TraceActor)
{
	if (TraceActor)
	{
		if (TracingTile)
		{
			if (TracingTile != TraceActor)
			{
				//UE_LOG(LogTemp, Warning, TEXT("TraceActor : %d"),(int32)TraceActor->GetTileState())
				if (SelectedCharacter && SelectedCharacter->isActivated() && (TraceActor->GetTileState() == ETileState::Movable || TraceActor->GetTileState() == ETileState::Path))
				{
					auto Controller = SelectedCharacter->GetController();
					if (Controller)
					{
						//UE_LOG(LogTemp,Warning,TEXT("Trace Actor : %s"),*TraceActor->GetName())
						// Reset previous PathTile's Material of Path State
						for (auto Tile : PathTile)
						{
							//UE_LOG(LogTemp, Warning, TEXT("Set Movable"))
							Tile->SetMovable();
						}
						Pathfinding(SelectedCharacter, TraceActor, PathTile);

						//UE_LOG(LogTemp, Warning, TEXT("Path : %d"),PathTile.Num())
						// Set the Material of Path State 
						for (auto Tile : PathTile)
						{
							Tile->SetPath();
						}
					}
				}
				else if(PathTile.Num()>0 && !(TraceActor->GetTileState() == ETileState::Movable))
				{
					auto Controller = SelectedCharacter->GetController();
					if (Controller)
					{
						for (auto Tile : PathTile)
						{
							//UE_LOG(LogTemp, Warning, TEXT("Set Movable"))
							Tile->SetMovable();
						}
						PathTile.Empty();
					}
				}
			}
			if (TracingTile == TraceActor) { return; }
			// If not Tracing State
			else if (
				TracingTile->GetTileState() != ETileState::Tracing &&
				TracingTile->GetTileState() != ETileState::TracingMovable &&
				TracingTile->GetTileState() != ETileState::TracingAttackable
				)
			{
				switch (TraceActor->GetTileState())
				{
					case ETileState::None:
						TracingTile = TraceActor;
						TracingTile->SetTracing();
						break;
					case ETileState::Movable:
						TracingTile = TraceActor;
						TracingTile->SetTracingMovable();
						break;
					case ETileState::Attackable:
						TracingTile = TraceActor;
						TracingTile->SetTracingAttackable();
						break;
				}
			}
			// Trace Actor : Current Tracing Tile
			if (TraceActor->GetTileState() == ETileState::None)
			{
				// Tracing Tile : Previous Tracing Tile
				// A : Tracing Tile -> B : Trace Actor (None)
				if (TracingTile->GetTileState() == ETileState::Tracing)
				{
					TracingTile->SetNoneOfState();
					TracingTile = TraceActor;
					TracingTile->SetTracing();
				}
				// TODO : Differentiate from Enemy Movable Tile
				else if (TracingTile->GetTileState() == ETileState::TracingMovable)
				{
					TracingTile->SetMovable();
					TracingTile = TraceActor;
					TracingTile->SetTracing();
				}
				// TODO : Differentiate from Enemy Attackable Tile
				else if (TracingTile->GetTileState() == ETileState::TracingAttackable)
				{
					TracingTile->SetAttackable();
					TracingTile = TraceActor;
					TracingTile->SetTracing();
				}
			}
			// A : Tracing Tile -> B : Trace Actor (Movable)
			else if (TraceActor->GetTileState() == ETileState::Movable)
			{
				if (TracingTile->GetTileState() == ETileState::Tracing)
				{
					TracingTile->SetNoneOfState();
					TracingTile = TraceActor;
					TracingTile->SetTracingMovable();
				}
				else if (TracingTile->GetTileState() == ETileState::TracingMovable)
				{
					TracingTile->SetMovable();
					TracingTile = TraceActor;
					TracingTile->SetTracingMovable();
				}
				else if (TracingTile->GetTileState() == ETileState::TracingAttackable)
				{
					TracingTile->SetAttackable();
					TracingTile = TraceActor;
					TracingTile->SetTracingMovable();
				}
				else if (TracingTile->GetTileState() == ETileState::Path)
				{
					TracingTile->SetMovable();
					TracingTile = TraceActor;
					TracingTile->SetTracingMovable();
				}
			}
			// A : Tracing Tile -> B : Trace Actor (Attackable)
			else if (TraceActor->GetTileState() == ETileState::Attackable)
			{
				if (TracingTile->GetTileState() == ETileState::Tracing)
				{
					TracingTile->SetNoneOfState();
					TracingTile = TraceActor;
					TracingTile->SetTracingAttackable();
				}
				else if (TracingTile->GetTileState() == ETileState::TracingMovable)
				{
					TracingTile->SetMovable();
					TracingTile = TraceActor;
					TracingTile->SetTracingAttackable();
				}
				else if (TracingTile->GetTileState() == ETileState::TracingAttackable)
				{
					TracingTile->SetAttackable();
					TracingTile = TraceActor;
					TracingTile->SetTracingAttackable();
				}
			}
			else if (TraceActor->GetTileState() == ETileState::Path)
			{
				if (TracingTile->GetTileState() == ETileState::Path)
				{
					TracingTile->SetMovable();
					TracingTile = TraceActor;
				}
				else if (TracingTile->GetTileState() == ETileState::TracingAttackable)
				{
					TracingTile->SetAttackable();
					TracingTile = TraceActor;
				}
			}

			//UE_LOG(LogTemp, Warning, TEXT("TraceActor2 : %d"), (int32)TraceActor->GetTileState())
		}// Tracing Tile
		else if(TraceActor->GetTileState()==ETileState::None)
		{
			TracingTile = TraceActor;
			// TODO : if(TraceActor!=ETileState::OnTheActor)
			TracingTile->SetTracing();
		}
	} // TraceActor
}

// TODO : Apply when Player is tracing tile
void AOperationIndigoPlayerController::Pathfinding(AOperationIndigoCharacter* SelectedCharacter, ATile * Target, TArray<ATile*>& PathMove)
{
	/** 4 Direction Pathfinding
	*	Get F,G,H Value in movable tiles to target
	*/

	/// Get F,G,H Value
	// Get Capsule Component from blueprint of Character
	// TODO : Why can apply 'const' to AOperationIndigoCharacter*?
	ATile* StartPointTile = SelectedCharacter->GetStartPointTile();
	if (StartPointTile)
	{
		TArray<ATile*> OpenList;
		TArray<ATile*> ClosedList;
		bool bIsCompleted = false;


		if (StartPointTile)
		{
			// Add CurrentTile to OpenList
			OpenList.Add(StartPointTile);
			while (OpenList.Num()>0)
			{
				ATile* CurrentTile = nullptr;

				// Find the lowest F Cost in OpenList and Select it as the Current Tile 
				for (auto Tile : OpenList)
				{
					if (
						!CurrentTile ||
						(CurrentTile->GetFCost() > Tile->GetFCost() ||
						(CurrentTile->GetFCost() == Tile->GetFCost() && CurrentTile->GetHCost() > Tile->GetHCost()))
						) 
						CurrentTile = Tile;
				}

				OpenList.Remove(CurrentTile);
				ClosedList.Add(CurrentTile);
				// if the current tile and the target are the same, set the bIsCompleted true
				if (CurrentTile == Target)
				{
					bIsCompleted = true;
					break;
				}

				TArray<ATile*> Neighbours = SelectedCharacter->GetNeighbourTile(CurrentTile);
				for (auto Tile : Neighbours)
				{
					bool bIsContainedInClosedList = false;
					for (auto ElementTile : ClosedList)
					{
						//UE_LOG(LogTemp, Warning, TEXT("Element Tile : %s"),*ElementTile->GetName())
						// Check that it is not in the closed list.
						if (Tile == ElementTile)
						{
							bIsContainedInClosedList = true;
							break;
						}
					}
					// This Tile is NOT contained in ClosedList
					if (!bIsContainedInClosedList)
					{
						bool bIsContainedInOpenList = false;
						for (auto ElementTile : OpenList)
						{
							if (Tile == ElementTile)
							{
								bIsContainedInOpenList = true;
								break;
							}
						}
						// This Tile is NOT contained in OpenList
						if (!bIsContainedInOpenList)
						{
							float CurrentPosX = CurrentTile->GetActorLocation().X, CurrentPosY = CurrentTile->GetActorLocation().Y;
							auto DistanceCount = Target->GetActorLocation() - Tile->GetActorLocation();
							int32 HCost = (FMath::Abs((int32)DistanceCount.X / 100) + FMath::Abs((int32)DistanceCount.Y / 100)) * 10;

							Tile->SetHCost(HCost);
							// Set G Cost
							Tile->SetGCost(CurrentTile->GetGCost() + 10);
							Tile->SetFCost(Tile->GetGCost() + HCost);

							Tile->SetParrentTile(CurrentTile);

							OpenList.Add(Tile);
						}
						// is Contained in OpenList
						else
						{
							if (Tile->GetGCost() > CurrentTile->GetGCost())
							{
								Tile->SetGCost(CurrentTile->GetGCost() + 10);
								Tile->SetFCost(Tile->GetGCost() + Tile->GetHCost());
								Tile->SetParrentTile(CurrentTile);
							}
						}
					}
				}
			}
			// if the current tile and the target are the same
			if (bIsCompleted)
			{
				TArray<ATile*> CompletedList;
				ATile* LastElem = nullptr;
				for (auto Tile : ClosedList)
				{
					if (Tile == Target) LastElem = Tile;
				}
				ATile* ParrentTile = LastElem->GetParrentTile();
				// Backtracking Parrent tile in ClosedList
				CompletedList.Add(LastElem);
				UE_LOG(LogTemp, Warning, TEXT("LastElem : %s"), *LastElem->GetName())
				UE_LOG(LogTemp, Warning, TEXT("ClosedList Num : %d"), ClosedList.Num())
				
				
				// Insert elements from ClosedList to CompletedList as the parrent tile
				for (int32 i = ClosedList.Num() - 2; i >= 0; i--)
				{
					LastElem = ParrentTile;
					ParrentTile = LastElem->GetParrentTile();
					CompletedList.Add(LastElem);
				}

				// Reverse CompletedList
				for (int32 i = CompletedList.Num() - 1; i> (CompletedList.Num() - 1) / 2; i--)
				{
					ATile* TempTile = CompletedList[CompletedList.Num() - 1 - i];
					CompletedList[CompletedList.Num() - 1 - i] = CompletedList[i];
					CompletedList[i] = TempTile;
				}

				/*for (auto Tile : CompletedList)
				{
					UE_LOG(LogTemp, Warning, TEXT("Tile : %s"),*Tile->GetName())
				}*/
				PathMove = CompletedList;
			}
		}
	}
}

void AOperationIndigoPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("Selection", IE_Pressed, this, &AOperationIndigoPlayerController::SelectionPressed);

	InputComponent->BindAction("RotateCamera", IE_Pressed, this, &AOperationIndigoPlayerController::RightMouseButton);
	InputComponent->BindAction("RotateCamera", IE_Released, this, &AOperationIndigoPlayerController::BranchReleased);
}

void AOperationIndigoPlayerController::SelectionPressed()
{
	if (bBattlePhase)
	{
		// Can't select multiple character
		FHitResult Hit;
		GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, Hit);
		auto HitCharacter = Cast<AOperationIndigoCharacter>(Hit.GetActor());

		if (!bActivatedUnit)
		{
			if (HitCharacter)
			{
				SelectCharacter(HitCharacter);
			}
			else
			{
				InitSelection();
			}
		}
	}
	else
	{
		// TODO : Can select multiple character
	}
}  

void AOperationIndigoPlayerController::SelectCharacter(AOperationIndigoCharacter * SelectCharacterToSet)
{
	InitSelection();
	SelectedCharacter = SelectCharacterToSet;
	SelectedCharacter->SetSelected();
}

void AOperationIndigoPlayerController::InitSelection()
{
	if (SelectedCharacter)
	{
		SelectedCharacter->SetDeSelected();
		SelectedCharacter = nullptr;
	}
}

const bool AOperationIndigoPlayerController::isBattlePhase()
{
	return bBattlePhase;
}

bool AOperationIndigoPlayerController::CheckPlayerController()
{
	if (SelectedCharacter)
	{
		auto PlayerAIController = Cast<APlayerAIController>(SelectedCharacter->GetController());
		if (PlayerAIController) { return true;	}
		else {return false;	}
	}
	else{return false;}
}

AOperationIndigoCharacter* AOperationIndigoPlayerController::GetActivatedCharacter() const
{
	if (bActivatedUnit)	return SelectedCharacter;
	else return nullptr;
}

void AOperationIndigoPlayerController::RightMouseButton()
{
	// Rotate the camera when Nobody is selected.
	if (!SelectedCharacter|| AIController)
	{
		// flag to set mouse event
		bRotatedCamera = true;
		bEnableClickEvents = false;
		bShowMouseCursor = false;
		bEnableMouseOverEvents = false;

		auto CameraPawn = Cast<ATacticalCamera>(GetPawn());
		CameraPawn->RotateCamera();
	}
	else if(SelectedCharacter && SelectedCharacter->isActivated())
	{
		FHitResult Hit;
		GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, Hit);
		if (Hit.Actor != NULL)
		{
			// Get Hit Tile
			auto DestinationTile = Cast<ATile>(Hit.GetActor());
			auto PlayerController = Cast<APlayerAIController>(SelectedCharacter->GetController());

			if (DestinationTile) {
				// Check if it is Movable Destination Tile
				if (DestinationTile->isMovable())
				{
					TracingTile = nullptr;
					//auto MoveLocation = DestinationTile->GetActorLocation();
					if (PlayerController && PathTile.Num()>0)
					{
						auto StartPointTile = SelectedCharacter->GetStartPointTile();
						TArray<ATile*> MovePath;
						if (StartPointTile)
						{
							ATile* CurrentTile = StartPointTile;
							FVector StartDirection = SelectedCharacter->GetActorForwardVector();
							UE_LOG(LogTemp, Warning, TEXT("Start Direction : %lf , %lf , %lf"), StartDirection.X, StartDirection.Y, StartDirection.Z);
							for (auto Tile : PathTile)
							{
								if (Tile != StartPointTile)
								{
									auto StartLocation = CurrentTile->GetActorLocation();
									auto MoveLocation = Tile->GetActorLocation();
									auto RelativeLocation = MoveLocation - StartLocation;
									auto VectorLength = FMath::Sqrt(FVector::DotProduct(RelativeLocation, RelativeLocation));
									auto Direction = RelativeLocation / VectorLength;
									/*UE_LOG(LogTemp, Warning, TEXT("Start Location : %lf ,  %lf,  %lf"), StartLocation.X, StartLocation.Y, StartLocation.Z);
									UE_LOG(LogTemp, Warning, TEXT("Move Location : %lf ,  %lf,  %lf"), MoveLocation.X, MoveLocation.Y, MoveLocation.Z);
									UE_LOG(LogTemp, Warning, TEXT("Relative Location : %lf ,  %lf,  %lf"), RelativeLocation.X, RelativeLocation.Y, RelativeLocation.Z);
									UE_LOG(LogTemp, Warning, TEXT("Vector Length : %lf"), VectorLength);
									UE_LOG(LogTemp, Warning, TEXT("Direction : %lf ,  %lf,  %lf"), Direction.X, Direction.Y, Direction.Z);*/
									
									if (StartDirection != Direction)
									{
										StartDirection = Direction;
										MovePath.Add(CurrentTile);
										UE_LOG(LogTemp,Warning,TEXT("Current Tile : %s"),*CurrentTile->GetName())
									}
									else if (Tile == DestinationTile)
									{
										MovePath.Add(Tile);
									}
									CurrentTile = Tile;
								}
							}
							if (MovePath.Num() > 0)
							{
								PlayerController->SetDestination(MovePath);
							}
							// SelectedCharacter->Pathfinding(DestinationTile);
							SelectedCharacter->ResetCollisionSphere();
						}
						
					}
				}

				// Check if it is Attackable Destination Tile
				if (DestinationTile->isAttackable())
				{
					// TODO : Store Enemy on the tile
					/*auto AttackToTarget = Cast<AOperationIndigoCharacter>(Hit.GetActor());
					if (AttackToTarget)
					{
						TracingTile = nullptr;
						auto EnemyAI = Cast<AEnemyAIController>(AttackToTarget->GetController());
						if (EnemyAI)
						{
							PlayerController->SetTargetToAttack(AttackToTarget);
						}
					}*/
				}
			} //Destination Tile

			else // Hit actor is not Tile
			{
				auto AttackToTarget = Cast<AOperationIndigoCharacter>(Hit.GetActor());
				if (AttackToTarget)
				{
					TracingTile = nullptr;
					auto EnemyAI = Cast<AEnemyAIController>(AttackToTarget->GetController());
					if (EnemyAI)
					{
						PlayerController->SetTargetToAttack(AttackToTarget);
						SelectedCharacter->ResetCollisionSphere();
					}
				}
			}
		} // Hit.Actor
	}
	//if character is selected, then actiavate MoveToTile
}

void AOperationIndigoPlayerController::BranchReleased()
{
	if (bRotatedCamera)
	{
		// return flag for mouse event
		bEnableClickEvents = true;
		bRotatedCamera = false;
		bShowMouseCursor = true;
		bEnableMouseOverEvents = true;

		// Deactivate Rotate Camera
		auto CameraPawn = Cast<ATacticalCamera>(GetPawn());
		CameraPawn->RotateReleased();
	}
}

void AOperationIndigoPlayerController::ActivateBattlePhase()
{
	bBattlePhase = true;
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AOperationIndigoCharacter* FindCharacter = Cast<AOperationIndigoCharacter>(*ActorItr);
		if (FindCharacter)
		{
			UnitsInBattlePhase.Add(FindCharacter);
			FindCharacter->MoveToShortestTile();
		}
	}
}

void AOperationIndigoPlayerController::DeActivateBattlePhase()
{
	bBattlePhase = false;
	// TODO : Delete Dead State Character

	for (auto Unit : UnitsInBattlePhase)
		Unit->StopGauge();

	UnitsInBattlePhase.Empty();
}