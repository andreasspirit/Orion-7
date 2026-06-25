// Copyright Epic Games, Inc. All Rights Reserved.

#include "SML_VirtualWorldGameMode.h"
#include "SML_VirtualWorldCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASML_VirtualWorldGameMode::ASML_VirtualWorldGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
