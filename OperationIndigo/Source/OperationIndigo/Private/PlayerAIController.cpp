// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerAIController.h"
#include "OperationIndigoCharacter.h"

void APlayerAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (ControlledCharacter && ControlledCharacter->isActivated() && ControlledCharacter->isSelected())
	{
		CheckOnAction();
		if (!bOnAction)
		{
			EndOfTurn();
		}
		else
		{
			// TODO : Replace to UEnum
			if (!bMoved && ControlledCharacter->isSelected() && ControlledCharacter->isActivated())
			{
				if (bOnAction)
				{
					MoveToTile();
				}
			}
			if (!bAttacked && ControlledCharacter->isSelected() && ControlledCharacter->isActivated())
			{
				// TODO : When I implement about Attack, delete comment
				/*if (bOnAction)
				{
					Attack();
				}*/
				bAttacked = true;
			}
		}//bOnAction
	}
}

void APlayerAIController::BeginPlay()
{
	Super::BeginPlay();
	ControlledCharacter = Cast<AOperationIndigoCharacter>(GetPawn());
}

void APlayerAIController::CheckOnAction()
{
	// TODO : Replace to UEnum
	if (bMoved && bAttacked)
	{
		bOnAction = false;
	}
	else if (!bMoved && !bAttacked)
	{
		bOnAction = true;
	}
	
}

void APlayerAIController::MoveToTile()
{
	if (ControlledCharacter->isActivated() && bSetLocation)
	{
		MoveToLocation(Location, 0.f);
		// TODO : True if the distance between the Character and Destination is the same
		bMoved = true;
		bSetLocation = false;
	}
}

void APlayerAIController::Attack()
{
	bAttacked = true;
}

// TODO : Link to HUD when EndTurn(Widget) is created
void APlayerAIController::EndOfTurn()
{
	bMoved = false;
	bAttacked = false;
	bSetLocation = false;
	ControlledCharacter->InitTurn();
}

void APlayerAIController::SetDestination(FVector MoveLocation)
{
	float Speed = ControlledCharacter->GetSpeed();
	Location = FMath::VInterpTo(ControlledCharacter->GetActorLocation(), MoveLocation, GetWorld()->GetTimeSeconds(), Speed);
	bSetLocation = true;
}
