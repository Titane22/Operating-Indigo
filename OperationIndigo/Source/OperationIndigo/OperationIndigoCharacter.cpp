// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "OperationIndigoCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Tile.h"
#include "Materials/Material.h"
#include "EngineMinimal.h"
#include "Engine.h"


AOperationIndigoCharacter::AOperationIndigoCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}



void AOperationIndigoCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
	if (!bStopGauge && ActionGauge < MaxGauge)
	{
		RiseGauge();
	}
	else if(ActionGauge>MaxGauge)
	{
		ReadyToStartTurn();
	}
	//UE_LOG(LogTemp, Warning, TEXT("%s Action Gauge : %lf"),*this->GetName(),ActionGauge)
}


void AOperationIndigoCharacter::InitCollisionSphere(USphereComponent* MovementToSet, USphereComponent* AttackToTset)
{
	MovementSphere = MovementToSet;
	AttackSphere = AttackToTset;
}

void AOperationIndigoCharacter::CollectGrids()
{
	// TODO : Change MoveRange System
	if (MovementSphere)
	{
		TArray<AActor*> Tiles;
		MovementSphere->bGenerateOverlapEvents = false;
		MovementSphere->GetOverlappingActors(Tiles);
		//AttackSphere->GetOverlappingActors(Tiles);

		for (auto Tile : Tiles)
		{
			auto TileToSet = Cast<ATile>(Tile);
			if (TileToSet)
			{
				TileToSet->SetMovable();
				Grid.Add(TileToSet);
			}
		}
	}
}

void AOperationIndigoCharacter::ResetCollisionSphere()
{
	if (MovementSphere)
	{
		MovementSphere = nullptr;
	}
	if (AttackSphere)
	{
		AttackSphere = nullptr;
	}
	// Initialize State of Tile in Grid
	for (auto Tile : Grid)
	{
		Tile->SetNoneOfState();
	}
	Grid.Empty();
}

void AOperationIndigoCharacter::InitTurn()
{
	bActivatedTurn = false;
	ActionGauge = 0.f;
	bCanAttack = false;
	bCanMove = false;
}

void AOperationIndigoCharacter::ReadyToStartTurn()
{
	bActivatedTurn = true;
	bCanAttack = true;
	bCanMove = true;
}

void AOperationIndigoCharacter::RiseGauge()
{
	ActionGauge += ActionGaugePer*GetWorld()->GetDeltaSeconds();
}

void AOperationIndigoCharacter::StopGauge()
{
	bStopGauge = true;
}

void AOperationIndigoCharacter::StartGauge()
{
	bStopGauge = false;
}

void AOperationIndigoCharacter::GenerateOverlapCollision()
{
	MovementSphere->bGenerateOverlapEvents = !MovementSphere->bGenerateOverlapEvents;
	AttackSphere->bGenerateOverlapEvents = !AttackSphere->bGenerateOverlapEvents;
}

const bool AOperationIndigoCharacter::isActivated(){ return bActivatedTurn;}
const bool AOperationIndigoCharacter::isSelected(){	return bSelected;}
const float AOperationIndigoCharacter::GetSpeed() { return GetCharacterMovement()->GetMaxSpeed(); }