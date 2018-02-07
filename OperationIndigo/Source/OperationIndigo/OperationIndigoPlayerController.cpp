// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "OperationIndigoPlayerController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "EngineUtils.h"
#include "HeadMountedDisplayFunctionLibrary.h"
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
	ActivateBattlePhase();
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
					PlayerCamera = GetPawn();
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

			
			//
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
		FHitResult TraceHit;
		
		// Grid control by Mouse Trace
		// Show Movable & Attackable Tile
		if (SelectedCharacter)
		{
			auto PlayerController = Cast<APlayerAIController>(SelectedCharacter->GetController());
			if (PlayerController)
			{
				auto Grids = SelectedCharacter->GetGrids();
				for (auto Tile : Grids)
				{
					ShowMovableTile(Tile);
				}
			}
		}
	}
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

void AOperationIndigoPlayerController::InitGridArray(TArray<ATile*> GridsToSet)
{
	//Grids = GridsToSet;
}

const bool AOperationIndigoPlayerController::isBattlePhase()
{
	return bBattlePhase;
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
			if (DestinationTile)
			{
				auto MoveLocation = DestinationTile->GetActorLocation();
				auto PlayerController = Cast<APlayerAIController>(SelectedCharacter->GetController());
				if (PlayerController)
				{
					PlayerController->SetDestination(MoveLocation);
				}
			}
		}
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
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AOperationIndigoCharacter* FindCharacter = Cast<AOperationIndigoCharacter>(*ActorItr);
		if (FindCharacter)
		{
			UnitsInBattlePhase.Add(FindCharacter);
		}
	}
}

void AOperationIndigoPlayerController::DeActivateBattlePhase()
{
	UnitsInBattlePhase.Empty();
}
