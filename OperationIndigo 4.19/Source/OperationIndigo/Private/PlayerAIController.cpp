// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerAIController.h"
#include "OperationIndigoCharacter.h"
#include "Tile.h"

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
				if (bOnAction)
				{
					Attack();
				}
			}
		}//bOnAction

		/// Temporary
		if (bTempMovingState)
		{
			EndTime += GetWorld()->GetDeltaSeconds();
			if (EndTime - StartTime > 1.5f)
			{
				bTempMovingState = false;
				ControlledCharacter->CollectGrids();
			}
		}

		if (bTempAttackingState)
		{
			EndTime += GetWorld()->GetDeltaSeconds();
			if (EndTime - StartTime > 1.5f)
			{
				bTempAttackingState = false;
				ControlledCharacter->CollectGrids();
			}
		}
		///
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
	if (bSetLocation)
	{
		MoveToLocation(Location, 0.f);
		// TODO : True if the distance between the Character and Destination is the same
		bMoved = true;
		bSetLocation = false;
		ControlledCharacter->MoveAction();
		/// temp
		bTempMovingState = true;
		EndTime = GetWorld()->GetDeltaSeconds();
		StartTime = EndTime;
		///
	}
}

void APlayerAIController::Attack()
{
	if (bSetTargetToAttack)
	{
		bAttacked = true;
		// Caculate Damage using 
		UE_LOG(LogTemp,Warning,TEXT("Attack %s !!! "),*Target->GetName())
		ControlledCharacter->AttackAction();
		bTempAttackingState = true;
	}
}

// TODO : Link to HUD when EndTurn(Widget) is created
void APlayerAIController::EndOfTurn()
{
	bMoved = false;
	bAttacked = false;
	bSetLocation = false;
	bSetTargetToAttack = false;

	Target = nullptr;
	ControlledCharacter->InitTurn();
}

void APlayerAIController::SetDestination(TArray<ATile*> Waypoints)
{
	float Speed = ControlledCharacter->GetSpeed();
	auto TileItr = Waypoints.CreateIterator();
	if(TileItr) UE_LOG(LogTemp,Warning,TEXT("Called"))
	for (auto Tile : Waypoints)
	{
		auto MoveLocation = Tile->GetActorLocation();
		UE_LOG(LogTemp, Warning, TEXT("Move Path's Tile : %s"), *Tile->GetName())
		Location = FMath::VInterpTo(ControlledCharacter->GetActorLocation(), MoveLocation, GetWorld()->GetTimeSeconds(), Speed);
	}
	bSetLocation = true;
}

void APlayerAIController::SetTargetToAttack(AOperationIndigoCharacter * TargetToAttack)
{
	// TODO : Set target to attack in the OIPlayerController
	Target = TargetToAttack;
	bSetTargetToAttack = true;
}

AOperationIndigoCharacter * APlayerAIController::GetControlledCharacter() const
{
	if (ControlledCharacter) { return ControlledCharacter; }
	else return nullptr;
}
