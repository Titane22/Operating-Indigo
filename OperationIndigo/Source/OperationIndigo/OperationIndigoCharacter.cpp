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

	CurrentHitPoint = InitialHitPoint;
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
}

float AOperationIndigoCharacter::GetCurrentHitPoint() const
{
	return CurrentHitPoint / InitialHitPoint;
}

void AOperationIndigoCharacter::UpdateCurrentHitPoint(float HitPoint)
{
	CurrentHitPoint -= (int32)HitPoint;
}

void AOperationIndigoCharacter::InitCollisionSphere(USphereComponent* MovementToSet, USphereComponent* AttackToSet)
{
	if (MovementToSet) {
		MovementSphere = MovementToSet;
	}
	if (AttackToSet) {
		AttackSphere = AttackToSet;
	}
}

void AOperationIndigoCharacter::MoveToShortestTile()
{
	/**
	* Get Collision Component 
	* Get Overlapping Tiles from collision 
	* Get Shortest Distance between the tile's actor location of first data  and character's actor location
	* Find the shortest distance and the tile of the shortest distance in loop
	* Move to the tile of the shortest distance
	*/
	TArray<AActor*> Tiles;
	UCapsuleComponent* CapsuleComponent = nullptr;
	
	auto Components = GetComponents();
	for (auto Component : Components)
	{
		if (Component->GetFName() == "Capsule")
		{
			CapsuleComponent = Cast<UCapsuleComponent>(Component);
		}
	}
	
	if (CapsuleComponent) 
	{
		CapsuleComponent->GetOverlappingActors(OUT Tiles);
		float ShortestDist;
		ATile* NearestTile = nullptr;

		if (Tiles.Num() > 0)
		{
			for (auto Tile : Tiles)
			{
				if (!NearestTile)
				{
					NearestTile = Cast<ATile>(Tile);
					FVector LocationDistance = GetActorLocation() - Tile->GetActorLocation();
					ShortestDist = LocationDistance.Size();
					//UE_LOG(LogTemp, Warning, TEXT("Distance : %lf"), DistanceLength)
				}
				else
				{
					auto CurrentTile = Cast<ATile>(Tile);
					FVector LocationDistance = GetActorLocation() - CurrentTile->GetActorLocation();
					float DistanceLength = LocationDistance.Size();
					UE_LOG(LogTemp, Warning, TEXT("Distance : %lf"), DistanceLength)
					if (ShortestDist > DistanceLength)
					{
						ShortestDist = DistanceLength;
						NearestTile = CurrentTile;
					}
				}
			}
			// TODO : Change smoothly move performance
			SetActorLocation(NearestTile->GetActorLocation());
			NearestTile->SetStepOn();
		}
	}
}

void AOperationIndigoCharacter::SetTargetLocation(FVector Location)
{
	EndLocation=Location;
	EndLocation.Z = GetActorLocation().Z;
}

void AOperationIndigoCharacter::Pathfinding(ATile * Target)
{
	TArray<AActor*> OpenList;
	TArray<AActor*> ClosedList;
}

void AOperationIndigoCharacter::CollectGrids()
{
	// TODO : Change MoveRange System
	if (MovementSphere && AttackSphere)
	{
		if (bCanMove && bCanAttack)
		{
			TArray<AActor*> Tiles;
			MovementSphere->SetSphereRadius(MovementRange);
			MovementSphere->GetOverlappingActors(OUT Tiles);

			for (auto Tile : Tiles)
			{
				auto TileToSet = Cast<ATile>(Tile);
				if (TileToSet)
				{
					TileToSet->SetMovable();
					Grid.Add(TileToSet);
				}
			}

			AttackSphere->SetSphereRadius(MovementRange + AttackRange);
			AttackSphere->GetOverlappingActors(OUT Tiles);

			for (auto Tile : Tiles)
			{
				auto TileToSet = Cast<ATile>(Tile);
				if (TileToSet && !TileToSet->isMovable())
				{
					TileToSet->SetAttackable();
					Grid.Add(TileToSet);
				}
			}
		}// bCanMove & bCanAttack
		else if (bCanMove && !bCanAttack)
		{
			TArray<AActor*> Tiles;
			MovementSphere->SetSphereRadius(MovementRange);
			MovementSphere->GetOverlappingActors(OUT Tiles);

			for (auto Tile : Tiles)
			{
				auto TileToSet = Cast<ATile>(Tile);
				if (TileToSet)
				{
					TileToSet->SetMovable();
					Grid.Add(TileToSet);
				}
			}
		}// bCanMove
		else if (!bCanMove && bCanAttack)
		{
			TArray<AActor*> Tiles;
			AttackSphere->SetSphereRadius(AttackRange);
			AttackSphere->GetOverlappingActors(OUT Tiles);

			for (auto Tile : Tiles)
			{
				auto TileToSet = Cast<ATile>(Tile);
				if (TileToSet)
				{
					TileToSet->SetAttackable();
					Grid.Add(TileToSet);
				}
			}
		}// bCanAttack
	}
}

void AOperationIndigoCharacter::ResetCollisionSphere()
{
	if (MovementSphere && !bCanMove)
	{
		MovementSphere = nullptr;
	}
	if (AttackSphere && !bCanAttack)
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
	EndLocation = FVector::FVector();
}

void AOperationIndigoCharacter::ReadyToStartTurn()
{
	if (!bActivatedTurn)
	{
		bActivatedTurn = true;
		bCanAttack = true;
		bCanMove = true;
	}
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

//void AOperationIndigoCharacter::MoveAction()
//{
//	if (bCanMove)
//	{
//		bCanMove = false;
//	}
//}

void AOperationIndigoCharacter::AttackAction()
{
	if (bCanAttack)
	{
		bCanAttack = false;
	}
}

const bool AOperationIndigoCharacter::isActivated(){ return bActivatedTurn;}
const bool AOperationIndigoCharacter::isSelected(){	return bSelected;}
const float AOperationIndigoCharacter::GetSpeed() { return GetCharacterMovement()->GetMaxSpeed(); }