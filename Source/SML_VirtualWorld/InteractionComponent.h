// InteractionComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

/**
 * Add this component to your player character.
 * It handles the interact key (E) and performs a line trace
 * to find actors that implement IInteractableInterface.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

protected:
	virtual void BeginPlay() override;

	// ??? Configuration ????????????????????????????????????????

	/** How far the interaction line trace reaches (in Unreal units). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionRange = 500.f;

	/** The input action for the interact key. Create an IA_Interact asset and assign it here. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Input")
	TObjectPtr<UInputAction> InteractAction;

	/** Optional: mapping context to add at BeginPlay. Leave empty if you already have one. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Input")
	TObjectPtr<UInputMappingContext> InteractionMappingContext;

	/** Priority for the mapping context (higher = takes precedence). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Input")
	int32 MappingContextPriority = 0;

private:
	/** Binds the interact action to the Enhanced Input system. */
	void SetupInput();

	/** Called when the interact key is pressed. */
	void OnInteract(const FInputActionValue& Value);

	/** Performs a line trace from the camera and returns the hit actor (if any). */
	AActor* TraceForInteractable() const;
};