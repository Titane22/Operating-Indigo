// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "OperationIndigoGameMode.h"
#include "OperationIndigoPlayerController.h"
#include "OperationIndigoCharacter.h"
#include "UObject/ConstructorHelpers.h"

AOperationIndigoGameMode::AOperationIndigoGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AOperationIndigoPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}