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

	if (bBattlePhase)
	{
		if (!bActivatedUnit)
		{
			
			// Find Activated Unit in UnitsInBattlePhase
			for (auto Unit : UnitsInBattlePhase)
			{
				if (Unit->isActivated())
				{
					ActivatedCharacter = Unit;
					ActivatedCharacter->SetSelected();
					bActivatedUnit = true;
					bStopGauge = true;

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
			// If ActivatedCharacter is deactivated, Find again.
			if (!ActivatedCharacter->isActivated())
			{
				InitActivation();
				bActivatedUnit = false;
			}
		}
	}
}

void AOperationIndigoPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("LeftMouseButton", IE_Pressed, this, &AOperationIndigoPlayerController::SelectionPressed);

	//InputComponent->BindAction("RightMouseButton", IE_Released, this, &AUntitledNamedPlayerController::MoveReleased);
	InputComponent->BindAction("RotateCamera", IE_Pressed, this, &AOperationIndigoPlayerController::RotateCamera);
	InputComponent->BindAction("RotateCamera", IE_Released, this, &AOperationIndigoPlayerController::BranchReleased);
}

void AOperationIndigoPlayerController::SelectionPressed()
{
	if (bBattlePhase)
	{

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("false"))
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

void AOperationIndigoPlayerController::InitActivation()
{
	if (ActivatedCharacter)
	{
		ActivatedCharacter->SetDeSelected();
		SelectedCharacter = nullptr;
	}
}

const bool AOperationIndigoPlayerController::isBattlePhase()
{
	return bBattlePhase;
}

void AOperationIndigoPlayerController::RotateCamera()
{
	if (!SelectedCharacter)
	{
		// flag to set mouse event
		bRotatedCamera = true;
		bEnableClickEvents = false;
		bShowMouseCursor = false;
		bEnableMouseOverEvents = false;

		auto CameraPawn = Cast<ATacticalCamera>(GetPawn());
		CameraPawn->RotateCamera();
	}
	else if(SelectedCharacter && SelectedCharacter==ActivatedCharacter)
	{
		FHitResult Hit;
		GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, Hit);
		if (Hit.Actor != NULL)
		{
			auto DestinationTile = Cast<ATile>(Hit.GetActor());
			if (DestinationTile)
			{
				auto MoveLocation = DestinationTile->GetActorLocation();
				UE_LOG(LogTemp,Warning,TEXT("MoveLocation : %s"),*MoveLocation.ToString())
				auto PlayerController = Cast<APlayerAIController>(SelectedCharacter->GetController());
				PlayerController->SetDestination(MoveLocation);
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
	for (TActorIterator<AOperationIndigoCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AOperationIndigoCharacter *FindCharacter = *ActorItr;
		UnitsInBattlePhase.Add(FindCharacter);
	}
}

void AOperationIndigoPlayerController::DeActivateBattlePhase()
{
	UnitsInBattlePhase.Empty();
}
