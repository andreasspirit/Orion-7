// InteractableInterface.h

#pragma once

#include "CoreMinimal.h"
#include "UObject\Interface.h"
#include "InteractableInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for any actor the player can interact with.
 * Implement this on your NPC (and later on doors, items, terminals, etc.)
 */
class IInteractableInterface
{
	GENERATED_BODY()

public:
	/** Called when the player presses the interact key while in range. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(APawn* InstigatorPawn);

	/** Returns the prompt text shown to the player (e.g. "Press E to talk"). */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractionPrompt() const;
};