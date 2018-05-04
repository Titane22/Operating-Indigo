// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAIController.h"
#include "OperationIndigoCharacter.h"

void AEnemyAIController::ChoiceAction()
{
	// TODO : Replace to UEnum and Need Refactoring
	
}

void AEnemyAIController::CheckOnAction()
{
	if (bMoved && bAttacked)
	{
		bOnAction = false;
	}

}

void AEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (ControlledCharacter && ControlledCharacter->isActivated())
	{
		// A temporary measure to show the Turn effect
		if (!bStartTurn)
		{
			StartTurnTime= FPlatformTime::Seconds();
		}
		else
		{
			EndTurnTime = FPlatformTime::Seconds();
		}

		CheckOnAction();
		if (!bOnAction && EndTurnTime - StartTurnTime>1.5f)
		{
			EndOfTurn();
		}
		else
		{
			CheckOnAction();
		}
		if (!bMoved && ControlledCharacter->isSelected() && ControlledCharacter->isActivated())
		{
			MoveToTile();
		}
		if (!bAttacked && ControlledCharacter->isSelected() && ControlledCharacter->isActivated())
		{
			Attack();
		}
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
	bStartTurn = false;
	ControlledCharacter->InitTurn();
}