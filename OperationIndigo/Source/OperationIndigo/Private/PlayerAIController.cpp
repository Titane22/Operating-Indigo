// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerAIController.h"
#include "OperationIndigoCharacter.h"

void APlayerAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bMoved && bAttacked)
	{
		EndOfTurn();
	}
	if (!bMoved && ControlledCharacter->isSelected() && ControlledCharacter->isActivated())
	{
		if (bOnAction)
		{
			MoveToTile();
		}
	}
	if (!bAttacked && ControlledCharacter->isSelected() && ControlledCharacter->isActivated())
	{
		if (bOnAction)
		{
			Attack();
		}
	}
}

void APlayerAIController::BeginPlay()
{
	Super::BeginPlay();
	ControlledCharacter = Cast<AOperationIndigoCharacter>(GetPawn());
}

void APlayerAIController::MoveToTile()
{
	if (ControlledCharacter->isActivated() && bSetLocation)
	{
		MoveToLocation(Location, 0.f);
		ControlledCharacter->InitTurn();
		bMoved = true;
		bSetLocation = false;
		if (bMoved && bAttacked)
		{
			bOnAction = false;
		}
	}
}

void APlayerAIController::Attack()
{
	bAttacked = true;
	if (bMoved && bAttacked)
	{
		bOnAction = false;
	}
}

// TODO : Link to HUD when EndTurn(Widget) is created
void APlayerAIController::EndOfTurn()
{
	bMoved = false;
	bAttacked = false;
	ControlledCharacter->InitTurn();
}

void APlayerAIController::SetDestination(FVector MoveLocation)
{
	float Speed = ControlledCharacter->GetSpeed();
	Location = FMath::VInterpTo(ControlledCharacter->GetActorLocation(), MoveLocation, GetWorld()->GetTimeSeconds(), Speed);
	bSetLocation = true;
}
