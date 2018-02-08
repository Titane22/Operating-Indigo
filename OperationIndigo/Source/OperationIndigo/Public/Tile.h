// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

UENUM(BlueprintType)
enum class ETileState : uint8
{
	Movable,
	Attackable,
	Obstacle,
	Tracing,
	OnTheActor,
	TracingMovable,
	TracingAttackable,
	None
};

UCLASS()
class OPERATIONINDIGO_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetMovable();

	void SetAttackable();

	void SetNoneOfState();

	void SetTracing();

	void SetObstacle();

	void SetOnTheActor();

	void SetTracingMovable();

	void SetTracingAttackable();

	bool isMovable();

	bool isAttackable();

	UFUNCTION(BlueprintCallable, Category="Grid")
	const ETileState GetTileState();
private:
	ETileState TileState = ETileState::None;
};
