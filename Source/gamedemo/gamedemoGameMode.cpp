// Copyright Epic Games, Inc. All Rights Reserved.

#include "gamedemoGameMode.h"
#include "gamedemoPlayerController.h"
#include "gamedemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

AgamedemoGameMode::AgamedemoGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AgamedemoPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}