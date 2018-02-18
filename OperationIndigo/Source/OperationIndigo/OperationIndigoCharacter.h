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

	// Gauge to activate turn
	float ActionGauge = 0.f;

	const float MaxGauge = 100.f;

	// it allows character to do action
	bool bActivatedTurn = false;

	bool bIsAttacked = false;

	UPROPERTY(EditAnywhere, Category = "SetUp")
	int32 MovementRange = 300;

	UPROPERTY(EditAnywhere, Category = "SetUp")
	int32 AttackRange = 300;

	USphereComponent* MovementSphere = nullptr;

	USphereComponent* AttackSphere = nullptr;

	UPROPERTY(EditAnywhere,  BlueprintReadWrite, Category = "Grid")
	TArray<ATile*> Grid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Select")
	bool bSelected = false;

	bool bStopGauge = false;

	bool bCanAttack = false;

	bool bCanMove = false;

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	//virtual void BeginPlay() override;
public:
	UFUNCTION(BlueprintCallable, Category = "Select")
	const bool isActivated();

	const bool isSelected();

	const float GetSpeed();

	AOperationIndigoCharacter();

	UFUNCTION(BlueprintImplementableEvent, Category = "Select")
	void SetSelected();

	UFUNCTION(BlueprintImplementableEvent, Category = "Select")
	void SetDeSelected();

	// initialize the Deactivate Turn State
	void InitTurn();
	
	void ReadyToStartTurn();

	// It called by OI-Controller.
	void RiseGauge();

	void StopGauge();

	void StartGauge();

	void GenerateOverlapCollision();

	void MoveAction();

	void AttackAction();

	UFUNCTION(BlueprintCallable, Category = "Select")
	void CollectGrids();

	UFUNCTION(BlueprintCallable, Category = "Select")
	void ResetCollisionSphere();

	UFUNCTION(BlueprintCallable, Category = "Select")
	void InitCollisionSphere(USphereComponent* MovementToSet, USphereComponent* AttackToTset);
};

