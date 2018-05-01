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
	StepOn,
	TracingMovable,
	TracingAttackable,
	None
	// Add a Tile state below the Enemy
};

USTRUCT()
struct FPathScoring
{
	GENERATED_USTRUCT_BODY()

	int32 FCost=0;
	int32 GCost=0;
	int32 HCost=0;
	class ATile* Parrent = nullptr;
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

	void SetStepOn();

	void SetTracingMovable();

	void SetTracingAttackable();

	// Returns true or false.
	bool isMovable();
	// Returns true or false.
	bool isAttackable();

	// Returh this tile state
	UFUNCTION(BlueprintCallable, Category="Grid")
	ETileState GetTileState() const;

	/// Set F,G,H values
	void SetFCost(int32 Value);
	void SetGCost(int32 Value);
	void SetHCost(int32 Value);
	void SetParrentTile(ATile* Parrent);

	/// Get F,G,H values
	int32 GetFCost() const;
	int32 GetGCost() const;
	int32 GetHCost() const;
	ATile* GetParrentTile() const;

private:
	// Initialize None state
	ETileState TileState = ETileState::None;

	FPathScoring PathScoring;
};
