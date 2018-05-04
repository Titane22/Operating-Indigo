// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TacticalCamera.generated.h"

UCLASS()
class OPERATIONINDIGO_API ATacticalCamera : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATacticalCamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Declaration to call in BP
	UFUNCTION(BlueprintImplementableEvent, Category = "Camera Setup")
	void RotateCamera();

	UFUNCTION(BlueprintImplementableEvent, Category = "Camera Setup")
	void RotateReleased();
	
};
