// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "OperationIndigoPlayerController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "EngineUtils.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "TacticalCamera.h"
#include "OperationIndigoCharacter.h"
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
		for (auto Unit : UnitsInBattlePhase)
		{
			if (!bStopGauge)
			{
				if (Unit->GetGauge() < 100)
				{
					Unit->RiseGauge();
				}
				else
				{
					Unit->bActivatedTurn = true;
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("%s Gauge Ouput: %lf"), *Unit->GetName(), Unit->GetGauge())
		}
		
		for (auto Unit : UnitsInBattlePhase)
		{
			if (Unit->bActivatedTurn)
			{
				Count++;
			}
		}

		if (Count != 0)
		{
			bStopGauge = true;
		}
		else
		{
			bStopGauge = false;
		}

		Count = 0;
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

void AOperationIndigoPlayerController::MoveToTile()
{
	
	if (SelectedCharacter->bActivatedTurn)
	{
		FHitResult Hit;
		GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, Hit);
		if (Hit.Actor != NULL)
		{
			auto DestinationTile = Cast<ATile>(Hit.GetActor());
			if (DestinationTile)
			{
				SelectedCharacter->MoveToTile(DestinationTile->GetActorLocation());
			}
		}
	}
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
	//if character is selected, then actiavate MoveToTile
	else if (SelectedCharacter)
	{
		MoveToTile();
	}
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
