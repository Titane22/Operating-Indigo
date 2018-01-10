// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OperationIndigoCharacter.generated.h"

UCLASS(Blueprintable)
class AOperationIndigoCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	// Gague Rise Per Seconds
	float ActionGaugePer = 10.f;

	// Gauge to activate turn
	float ActionGauge = 0.f;

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
public:
	// it allows character to do action
	bool bActivatedTurn = false;
	
	// it control Gauge
	bool bCanRunGauge = true;

	const float GetGauge();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Select")
	bool bSelected = false;

	AOperationIndigoCharacter();

	UFUNCTION(BlueprintImplementableEvent, Category = "Select")
	void SetSelected();

	UFUNCTION(BlueprintImplementableEvent, Category = "Select")
	void SetDeSelected();

	// Move character to Tile
	void MoveToTile(FVector Location);
	// initialize the Deactivate Turn State
	void InitTurn();
	
	// It called by OIController.
	void RiseGauge();
};

