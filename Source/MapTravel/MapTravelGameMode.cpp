// Copyright Epic Games, Inc. All Rights Reserved.

#include "MapTravelGameMode.h"
#include "MapTravelCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMapTravelGameMode::AMapTravelGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
