// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PlayerAIController.generated.h"

/**
 * 
 */

class AOperationIndigoCharacter;

UCLASS()
class OPERATIONINDIGO_API APlayerAIController : public AAIController
{
	GENERATED_BODY()
private:
	AOperationIndigoCharacter* ControlledCharacter;

	bool bOnAction = true;

	bool bMoved = false;
	bool bTempMovingState = false; // temporary
	bool bTempAttackingState = false;
	float StartTime;
	float EndTime;

	bool bAttacked = false;

	bool bSetLocation = false;

	bool bSetTargetToAttack = false;

	FVector Location;

	AOperationIndigoCharacter* Target = nullptr;
public:
	virtual void Tick(float DeltaSeconds) override;
	void CheckOnAction();
	virtual void BeginPlay() override;
	// Move character to Tile
	void MoveToTile();
	
	void Attack();
	
	UFUNCTION(BlueprintCallable, Category = "Select")
	void EndOfTurn();

	void SetDestination(FVector MoveLocation);

	void SetTargetToAttack(AOperationIndigoCharacter* TargetToAttack);

	UFUNCTION(BlueprintCallable,Category="Setup")
	AOperationIndigoCharacter* GetControlledCharacter() const;
};
