// PickupItem.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "PickupItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;

/**
 * Base class for any item the player can pick up by pressing E.
 * Create Blueprint children of this for each pickup type.
 */
UCLASS()
class APickupItem : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	APickupItem();

protected:
	virtual void BeginPlay() override;

	// ??? Components ???????????????????????????????????????????

	/** The visible mesh of the item. Assign in Blueprint. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	/** Small collision sphere so the line trace can hit the item. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComp;

	// ??? Configuration ????????????????????????????????????????

	/** Name shown in the interaction prompt. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	FString ItemName = TEXT("Item");

	/** Tag added to the player's inventory when picked up. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	FName InventoryTag = TEXT("Item");

	// ??? Interactable Interface ???????????????????????????????

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
	virtual FText GetInteractionPrompt_Implementation() const override;

	// ??? Pickup logic ?????????????????????????????????????????

	/** Called when the player picks up this item. Override in subclasses. */
	UFUNCTION(BlueprintNativeEvent, Category = "Pickup")
	void OnPickedUp(APawn* InstigatorPawn);
	virtual void OnPickedUp_Implementation(APawn* InstigatorPawn);
};