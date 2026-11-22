// Copyright Epic Games, Inc. All Rights Reserved.


#include "SML_VirtualWorldPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameStateManager.h"
#include "Engine/Engine.h"

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

void ASML_VirtualWorldPlayerController::ShowGameState()
{
	UGameStateManager* State = GetGameInstance()->GetSubsystem<UGameStateManager>();
	if (!State)
	{
		return;
	}

	const FString Context = State->BuildContextString();
	UE_LOG(LogTemp, Log, TEXT("\n%s"), *Context);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, Context);
	}
}