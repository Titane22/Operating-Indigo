// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "OperationIndigoCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "PlayerAIController.h"

const float AOperationIndigoCharacter::GetGauge(){ return ActionGauge;}

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
}

void AOperationIndigoCharacter::MoveToTile(FVector Location)
{
	if (bActivatedTurn)
	{
		auto Controller = Cast<APlayerAIController>(GetController());
		if (Controller)
		{
			auto MoveToLocation = FMath::VInterpTo(this->GetActorLocation(), Location, GetWorld()->GetTimeSeconds(), GetCharacterMovement()->GetMaxSpeed());
			Controller->MoveToLocation(MoveToLocation, 0.f);
			bActivatedTurn = false;
			ActionGauge = 0.f;
		}
	}
}

void AOperationIndigoCharacter::InitTurn()
{
	bActivatedTurn = true;
}

void AOperationIndigoCharacter::RiseGauge()
{
	ActionGauge += ActionGaugePer*GetWorld()->GetDeltaSeconds();
}
