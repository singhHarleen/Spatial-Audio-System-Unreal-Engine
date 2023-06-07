// Copyright Epic Games, Inc. All Rights Reserved.

#include "CstSpatialAudioGameMode.h"
#include "CstSpatialAudioCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACstSpatialAudioGameMode::ACstSpatialAudioGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
