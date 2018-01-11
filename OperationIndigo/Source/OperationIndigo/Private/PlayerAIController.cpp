// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerAIController.h"
#include "OperationIndigoCharacter.h"

void APlayerAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void APlayerAIController::BeginPlay()
{
	Super::BeginPlay();
	ControlledCharacter = Cast<AOperationIndigoCharacter>(GetPawn());
}

void APlayerAIController::MoveToTile(FVector Location)
{
	if (ControlledCharacter->isActivated())
	{
		float Speed = ControlledCharacter->GetSpeed();
		auto MoveLocation = FMath::VInterpTo(ControlledCharacter->GetActorLocation(), Location, GetWorld()->GetTimeSeconds(), Speed);
		MoveToLocation(MoveLocation, 0.f);
		ControlledCharacter->InitTurn();
	}
}
// TODO : Link to HUD when EndTurn(Widget) is created
void APlayerAIController::EndOfTurn()
{
	ControlledCharacter->InitTurn();
}