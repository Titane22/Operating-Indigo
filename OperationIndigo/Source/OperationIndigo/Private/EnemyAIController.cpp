// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAIController.h"
#include "OperationIndigoCharacter.h"

void AEnemyAIController::ChoiceAction()
{
	if (!bMoved && ControlledCharacter->isSelected() && ControlledCharacter->isActivated())
	{
		MoveToTile();
	}
	if (!bAttacked && ControlledCharacter->isSelected() && ControlledCharacter->isActivated())
	{
		Attack();
	}
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bMoved && bAttacked)
	{
		EndOfTurn();
	}
	else
	{
		ChoiceAction();
	}
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	ControlledCharacter = Cast<AOperationIndigoCharacter>(GetPawn());
}

void AEnemyAIController::MoveToTile()
{
	bMoved = true;
}

void AEnemyAIController::Attack()
{
	bAttacked = true;
}

// TODO : Link to HUD when EndTurn(Widget) is created
void AEnemyAIController::EndOfTurn()
{
	bMoved = false;
	bAttacked = false;
	ControlledCharacter->InitTurn();
}