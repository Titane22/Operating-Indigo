// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
class AOperationIndigoCharacter;

UCLASS()
class OPERATIONINDIGO_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
private:
	AOperationIndigoCharacter* ControlledCharacter;

	bool bOnAction = true;

	bool bMoved = false;

	bool bAttacked = false;

	void ChoiceAction();

	void CheckOnAction();
	
	void MoveToTile();

	void Attack();

	void EndOfTurn();
public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	// Move character to Tile
	
};
