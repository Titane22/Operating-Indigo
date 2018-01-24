// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerAIController.h"
#include "OperationIndigoCharacter.h"

void APlayerAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (ControlledCharacter && ControlledCharacter->isActivated())
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
				/*if (bOnAction)
				{
					MoveToTile();
				}*/
				if (bSetLocation)
					bMoved = true;
			}
			if (!bAttacked && ControlledCharacter->isSelected() && ControlledCharacter->isActivated())
			{
				/*if (bOnAction)
				{
					Attack();
				}*/
				bAttacked = true;
			}
		}//bOnAction
	}
}

void APlayerAIController::CheckOnAction()
{
	/*UE_LOG(LogTemp, Warning, TEXT("On Action : %d"), bOnAction)
	UE_LOG(LogTemp, Warning, TEXT("%s Moved : %d		Attacked : %d"), *ControlledCharacter->GetName(),bMoved,bAttacked)
	if(ControlledCharacter)
		UE_LOG(LogTemp, Warning, TEXT("Activated Turn : %d"), ControlledCharacter->isActivated())*/
	
	if (bMoved && bAttacked)
	{
		bOnAction = false;
	}
	else if (!bMoved && !bAttacked)
	{
		bOnAction = true;
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
	UE_LOG(LogTemp, Warning, TEXT("%s Activated Turn "), *ControlledCharacter->GetName())
	ControlledCharacter->InitTurn();
}

void APlayerAIController::SetDestination(FVector MoveLocation)
{
	float Speed = ControlledCharacter->GetSpeed();
	Location = FMath::VInterpTo(ControlledCharacter->GetActorLocation(), MoveLocation, GetWorld()->GetTimeSeconds(), Speed);
	bSetLocation = true;
}
