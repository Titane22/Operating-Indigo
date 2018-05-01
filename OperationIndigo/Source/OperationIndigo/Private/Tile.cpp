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

void ATile::SetTracingMovable()
{
	TileState = ETileState::TracingMovable;
}

void ATile::SetTracingAttackable()
{
	TileState = ETileState::TracingAttackable;
}

void ATile::SetPath()
{
	TileState = ETileState::Path;
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

void ATile::SetFCost(int32 Value) { PathScoring.FCost = Value; }

void ATile::SetGCost(int32 Value) { PathScoring.GCost = Value; }

void ATile::SetHCost(int32 Value) { PathScoring.HCost = Value; }

void ATile::SetParrentTile(ATile * Parrent) { PathScoring.Parrent = Parrent; }

int32 ATile::GetFCost() const { return PathScoring.FCost; }

int32 ATile::GetGCost() const { return PathScoring.GCost; }

int32 ATile::GetHCost() const { return PathScoring.HCost; }

ATile * ATile::GetParrentTile() const { return PathScoring.Parrent; }
