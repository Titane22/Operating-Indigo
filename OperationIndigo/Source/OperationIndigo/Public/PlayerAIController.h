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

	bool bAttacked = false;

	bool bSetLocation = false;

	FVector Location;
public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	// Move character to Tile
	void MoveToTile();
	
	void Attack();
	
	void EndOfTurn();

	void SetDestination(FVector MoveLocation);
};
