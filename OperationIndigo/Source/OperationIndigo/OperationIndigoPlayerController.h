// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "OperationIndigoPlayerController.generated.h"

class AOperationIndigoCharacter;
class ATacticalCamera;
class ABattleHUD;
class AEnemyAIController;
class ATile;

UCLASS()
class AOperationIndigoPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AOperationIndigoPlayerController();

	void SelectCharacter(AOperationIndigoCharacter* SelectCharacterToSet);

	void InitSelection();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	const bool isBattlePhase();

	// Shows material change of tile according to ETileState
	UFUNCTION(BlueprintImplementableEvent, Category = "Grid")
	void ShowStateOfTile();

	UFUNCTION(BlueprintImplementableEvent, Category = "UMG")
	void ShowEndTurn();

	UFUNCTION(BlueprintCallable, Category = "Setup")
	bool CheckPlayerController();

	UFUNCTION(BlueprintCallable, Category = "Setup")
	AOperationIndigoCharacter* GetActivatedCharacter() const;
protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface
	
	void SelectionPressed();

	void RotateCamera();

	void BranchReleased();

	bool bRotatedCamera = false;

	// TODO: Control for Battle Phase
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SetUp")
	bool bBattlePhase = true;

	void ActivateBattlePhase();

	void DeActivateBattlePhase();

	UPROPERTY(BlueprintReadOnly, Category = "SetUp")
	bool bActivatedUnit = false;

	bool bStopGauge = false;

	APawn* PlayerCamera=nullptr;

	TArray<AOperationIndigoCharacter*> UnitsInBattlePhase; // TODO : if bBattlePhase is false, then add to all character

	TArray<ATile*> TilesInBattlePhase;

	UPROPERTY(BlueprintReadOnly, Category = "SetUp")
	AOperationIndigoCharacter* SelectedCharacter = nullptr;

	AEnemyAIController* AIController = nullptr;	

	void GridTracingControl();

	void EstimateTileState(ATile * TraceActor);
private:
	ATile* TracingTile=nullptr;
};


