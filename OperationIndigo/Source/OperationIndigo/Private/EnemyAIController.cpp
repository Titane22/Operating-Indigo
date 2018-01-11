// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAIController.h"
#include "OperationIndigoCharacter.h"

void AEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	ControlledCharacter = Cast<AOperationIndigoCharacter>(GetPawn());
}

void AEnemyAIController::MoveToTile(FVector Location)
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
void AEnemyAIController::EndOfTurn()
{
	ControlledCharacter->InitTurn();
}