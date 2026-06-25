// Copyright Epic Games, Inc. All Rights Reserved.


#include "SML_VirtualWorldPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

void ASML_VirtualWorldPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// add the mapping context so we get controls
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
	// Reset input mode from main menu's UI-only mode
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
}