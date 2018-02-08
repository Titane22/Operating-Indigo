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

void ATile::SetOnTheActor()
{
	TileState = ETileState::OnTheActor;
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
	if (TileState == ETileState::Movable || TileState == ETileState::TracingMovable)
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
	if (TileState == ETileState::Attackable || TileState == ETileState::TracingAttackable)
	{
		return true;
	}
	else
	{
		return false;
	}
}

const ETileState ATile::GetTileState()
{
	return TileState;
}


