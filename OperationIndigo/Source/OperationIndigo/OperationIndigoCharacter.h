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

	// it allows character to do action
	bool bActivatedTurn = false;

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
public:
	const bool isActivated();

	const float GetGauge();

	const float GetSpeed();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Select")
	bool bSelected = false;

	AOperationIndigoCharacter();

	UFUNCTION(BlueprintImplementableEvent, Category = "Select")
	void SetSelected();

	UFUNCTION(BlueprintImplementableEvent, Category = "Select")
	void SetDeSelected();

	// initialize the Deactivate Turn State
	void InitTurn();
	
	void ActivatedTurn();
	// It called by OIController.
	void RiseGauge();
};

