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

	// Initialize SelectedCharacter
	void InitSelection();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	const bool isBattlePhase();

	// Shows material change of tile according to ETileState
	UFUNCTION(BlueprintImplementableEvent, Category = "Grid")
	void ShowStateOfTile();

	UFUNCTION(BlueprintCallable, Category = "Setup")
	bool CheckPlayerController();

	UFUNCTION(BlueprintCallable, Category = "Setup")
	AOperationIndigoCharacter* GetActivatedCharacter() const;

	// Activate Battle Phase, Get All Characters in to UnitsInBattlePhase
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void ActivateBattlePhase();
protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface
	
	// Left Mouse button Pressed
	void SelectionPressed();

	// Right Mouse Button Pressed
	void RightMouseButton();

	// Right Mouse button Released
	void BranchReleased();

	// Variable for controlling the gauge
	bool bRotatedCamera = false;

	// Control for Battle Phase
	bool bBattlePhase = false;

	// Deactivate Battle Phase, Empty the UnitsInBattlePhase
	void DeActivateBattlePhase();

	UPROPERTY(BlueprintReadOnly, Category = "SetUp")
	bool bActivatedUnit = false;

	// Bool for stopping the gauge
	bool bStopGauge = false;

	UPROPERTY(BlueprintReadOnly, Category = "SetUp")
	TArray<AOperationIndigoCharacter*> UnitsInBattlePhase; // TODO : if bBattlePhase is false, then add all character

	UPROPERTY(BlueprintReadOnly, Category = "SetUp")
	AOperationIndigoCharacter* SelectedCharacter = nullptr;

	// Find AI Controller from Current Selected Character
	AEnemyAIController* AIController = nullptr;	

	// Show Tile's Material according to Grid state.
	void GridTracingControl();

	void EstimateTileState(ATile * TraceActor);

	TArray<ATile*> PathTile;

	void Pathfinding(AOperationIndigoCharacter* SelectedCharacter,ATile* Target,TArray<ATile*>& PathTile);
private:
	// Current tracing tile
	ATile* TracingTile=nullptr;
};
