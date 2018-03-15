// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"


// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATile::SetMovable()
{
	TileState = ETileState::Movable;
}

void ATile::SetAttackable()
{
	TileState = ETileState::Attackable;
}

void ATile::SetNoneOfState()
{
	TileState = ETileState::None;
}

void ATile::SetTracing()
{
	TileState = ETileState::Tracing;
}

void ATile::SetObstacle()
{
	TileState = ETileState::Obstacle;
}

void ATile::SetStepOn()
{
	TileState = ETileState::StepOn;
}

void ATile::SetTracingMovable()
{
	TileState = ETileState::TracingMovable;
}

void ATile::SetTracingAttackable()
{
	TileState = ETileState::TracingAttackable;
}

bool ATile::isMovable()
{
	if (TileState == ETileState::Movable ||
		TileState == ETileState::TracingMovable)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ATile::isAttackable()
{
	if (TileState == ETileState::Attackable ||
		TileState == ETileState::TracingAttackable)
	{
		return true;
	}
	else
	{
		return false;
	}
}

ETileState ATile::GetTileState() const
{
	return TileState;
}

void ATile::SetFValue(int32 Value) { PathScoring.FValue = Value; }

void ATile::SetGValue(int32 Value) { PathScoring.GValue = Value; }

void ATile::SetHValue(int32 Value) { PathScoring.HValue = Value; }

int32 ATile::GetFValue() const { return PathScoring.FValue; }

int32 ATile::GetGValue() const { return PathScoring.GValue; }

int32 ATile::GetHValue() const { return PathScoring.HValue; }
