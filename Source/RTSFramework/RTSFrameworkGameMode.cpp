// Copyright Epic Games, Inc. All Rights Reserved.

#include "RTSFrameworkGameMode.h"
#include "RTSFrameworkCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARTSFrameworkGameMode::ARTSFrameworkGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
