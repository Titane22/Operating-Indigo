// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OperationIndigoCharacter.generated.h"

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Idle,
	Moving,
	Attacking
};

class ATile;
UCLASS(Blueprintable)
class AOperationIndigoCharacter : public ACharacter
{
	GENERATED_BODY()
protected:
	
	// Gague Rise Per Seconds
	float ActionGaugePer = 20.f;

	// Inital Gauge to activate turn
	float ActionGauge = 0.f;

	// Max Action Gauge
	const float MaxGauge = 100.f;

	// It allows character to do action
	bool bActivatedTurn = false;

	// The Character Inital Hit Point
	UPROPERTY(EditAnywhere, Category = "Character")
	float InitialHitPoint = 100.f;

	// The Character Current Hit Point
	UPROPERTY(EditAnywhere, Category = "Character")
	float CurrentHitPoint;

	// Character's Movable Range 
	UPROPERTY(EditAnywhere, Category = "SetUp")
	int32 MovementRange = 300;

	// Character's Attackable Range 
	UPROPERTY(EditAnywhere, Category = "SetUp")
	int32 AttackRange = 300;

	/**Set Collision Sphere to shot movable range & attackable range*/
	USphereComponent* MovementSphere = nullptr;
	USphereComponent* AttackSphere = nullptr;

	// Set the tiles in the game of battle phase
	UPROPERTY(EditAnywhere,  BlueprintReadWrite, Category = "Grid")
	TArray<ATile*> Grid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Select")
	bool bSelected = false;

	bool bStopGauge = true;

	/** Control Character State */
	bool bCanAttack = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bCanMove = false;
	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	// virtual void BeginPlay() override;
public:
	UPROPERTY(BlueprintReadOnly,Category="Movement")
	FVector EndLocation;
	// Constructor
	AOperationIndigoCharacter();

	// Accessor function for Current HitPoint
	UFUNCTION(BlueprintPure, Category = "Character")
	float GetCurrentHitPoint() const;

	// Update the Character's Current HitPoint
	UFUNCTION(BlueprintCallable, Category = "Character")
	void UpdateCurrentHitPoint(float HitPoint);

	// Return bActivatedTurn
	UFUNCTION(BlueprintCallable, Category = "Select")
	const bool isActivated();

	// Return bSelected
	const bool isSelected();

	const float GetSpeed();

	UFUNCTION(BlueprintImplementableEvent, Category = "Select")
	void SetSelected();

	UFUNCTION(BlueprintImplementableEvent, Category = "Select")
	void SetDeSelected();

	// initialize the Deactivate Turn State
	void InitTurn();
	
	// Activate Turn of this Character
	void ReadyToStartTurn();

	// It called by OI-Controller.
	void RiseGauge();

	// Stop to rise Gauge, Control bStop Gauge
	void StopGauge();

	// Start to rise Gauge, Control bStop Gauge
	void StartGauge();

	// Control Character's State about Movement
	UFUNCTION(BlueprintImplementableEvent,Category="Movement")
	void MoveAction();

	// Control Character's State about Attack
	void AttackAction();

	// When Character is Ativated, Collect Grid from Collision Sphere to Grid TArray
	UFUNCTION(BlueprintCallable, Category = "Select")
	void CollectGrids();

	// When Character is DeActivated, Empty the Grid & MovementSphere or AttackSphere
	UFUNCTION(BlueprintCallable, Category = "Select")
	void ResetCollisionSphere();

	// When Character is Activated, Initialize MovementSphere or AttackSphere from Blueprint
	UFUNCTION(BlueprintCallable, Category = "Select")
	void InitCollisionSphere(USphereComponent* MovementToSet, USphereComponent* AttackToTset);

	//When Game is activate the battle phase, move to the shortest tile
	void MoveToShortestTile();

	void SetTargetLocation(FVector Location);

	UCapsuleComponent * GetCapsule();

	TArray<ATile*> GetNeighbourTile(ATile* CurrentTile);
};

