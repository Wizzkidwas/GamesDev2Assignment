// Copyright Epic Games, Inc. All Rights Reserved.

#include "GamesDev2AssignmentGameMode.h"
#include "GamesDev2AssignmentCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGamesDev2AssignmentGameMode::AGamesDev2AssignmentGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
