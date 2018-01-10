// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OperationIndigoPlayerController.generated.h"

class AOperationIndigoCharacter;
class ATacticalCamera;

UCLASS()
class AOperationIndigoPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AOperationIndigoPlayerController();

	UFUNCTION(BlueprintCallable, Category = "Select")
	void SelectCharacter(AOperationIndigoCharacter* SelectCharacterToSet);

	UFUNCTION(BlueprintCallable, Category = "Select")
	void InitSelection();

	void MoveToTile();

	const float HeatBarGauge = 100.f;
protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface
	
	UFUNCTION(BlueprintCallable, Category = "Select")
	void SelectionPressed();

	void RotateCamera();

	void BranchReleased();

	TArray<AOperationIndigoCharacter*> UnitsInBattlePhase; // TODO : if bBattlePhase is false, then add to all character

	UPROPERTY(BlueprintReadOnly, Category = "SetUp")
	AOperationIndigoCharacter* SelectedCharacter = nullptr;

	bool bRotatedCamera = false;
	// TODO: Controll for Battle Phase
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SetUp")
	bool bBattlePhase = true;

	void ActivateBattlePhase();

	void DeActivateBattlePhase();

	int32 Count = 0;

	bool bStopGauge = false;
};


