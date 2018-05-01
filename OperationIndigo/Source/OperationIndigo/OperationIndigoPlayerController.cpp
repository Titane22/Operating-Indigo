// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "OperationIndigoPlayerController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
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
		}// Tracing Tile
		else if(TraceActor->GetTileState()==ETileState::None)
		{
			TracingTile = TraceActor;
			// TODO : if(TraceActor!=ETileState::OnTheActor)
			TracingTile->SetTracing();
		}
	} // TraceActor
}

void AOperationIndigoPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("Selection", IE_Pressed, this, &AOperationIndigoPlayerController::SelectionPressed);

	InputComponent->BindAction("RotateCamera", IE_Pressed, this, &AOperationIndigoPlayerController::RotateCamera);
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

void AOperationIndigoPlayerController::RotateCamera()
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
					auto MoveLocation = DestinationTile->GetActorLocation();
					if (PlayerController)
					{
						PlayerController->SetDestination(MoveLocation);
						SelectedCharacter->Pathfinding(DestinationTile);
						SelectedCharacter->ResetCollisionSphere();
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