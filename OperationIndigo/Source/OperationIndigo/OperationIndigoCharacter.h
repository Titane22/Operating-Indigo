// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OperationIndigoCharacter.generated.h"

UCLASS(Blueprintable)
class AOperationIndigoCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	float HearBarPer = 1.f;

public:
	bool bActivatedTurn = true;

	UPROPERTY(EditAnywhere, Category = "SetUp")
		int32 Priority;

	AOperationIndigoCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Select")
		void SetSelected();

	UFUNCTION(BlueprintImplementableEvent, Category = "Select")
		void SetDeSelected();

	float GetSpeed() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Select")
		bool bSelected = false;

	void MoveToTile(FVector Location);

	void InitTurn();
};

