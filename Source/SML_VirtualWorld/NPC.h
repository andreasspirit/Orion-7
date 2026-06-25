// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "NPC.generated.h"

class USphereComponent;
class USkeletalMeshComponent;
class UWidgetComponent;
class ULLMConnector;

UCLASS()
class SML_VIRTUALWORLD_API ANPC : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** The NPC's visible body. Swap for a SkeletalMesh when you have animations. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	/** Sphere trigger — when the player enters this, the interaction prompt appears. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> InteractionSphere;

	/** Optional floating widget above the NPC's head (shows name or "Press E"). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> OverheadWidgetComp;

	// ─── Configuration ────────────────────────────────────────

	/** Display name shown in the dialogue UI header. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Identity")
	FString NPCName = TEXT("Villager");

	/** System prompt sent to the LLM that defines this NPC's personality.
	 *  Example: "You are a gruff blacksmith named Gorm. You speak in short sentences." */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Identity", meta = (MultiLine = true))
	FString SystemPrompt = TEXT("You are a friendly NPC in a virtual world. Stay in character.");

	/** How close the player must be to interact (in Unreal units). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Interaction")
	float InteractionRadius = 300.f;

	/** The dialogue widget class to spawn when conversation starts.
	 *  Set this in the Blueprint defaults to your WBP_DialogueUI. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|UI")
	TSubclassOf<UUserWidget> DialogueWidgetClass;

	// ─── State ────────────────────────────────────────────────

	/** Reference to the currently open dialogue widget (nullptr if closed). */
	UPROPERTY()
	TObjectPtr<UUserWidget> ActiveDialogueWidget;

	/** The pawn currently inside the interaction sphere. */
	UPROPERTY()
	TObjectPtr<APawn> NearbyPawn;

	/** True while the dialogue UI is open. */
	UPROPERTY(BlueprintReadOnly, Category = "NPC|State")
	bool bIsInConversation = false;

	// ─── Overlap Events ───────────────────────────────────────

	UFUNCTION()
	void OnInteractionSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractionSphereEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	// ─── Interactable Interface ───────────────────────────────

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
	virtual FText GetInteractionPrompt_Implementation() const override;

	// ─── Dialogue Lifecycle ───────────────────────────────────

	/** Opens the dialogue widget and switches input mode. */
	UFUNCTION(BlueprintCallable, Category = "NPC|Dialogue")
	void StartConversation(APawn* InstigatorPawn);

	/** Closes the dialogue widget and restores game input. */
	UFUNCTION(BlueprintCallable, Category = "NPC|Dialogue")
	void EndConversation();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<ULLMConnector> LLMConnector;


	// In the header (protected section):
	UFUNCTION()
	void HandlePlayerMessage(const FString& Message);

	UFUNCTION()
	void HandleLLMResponse(const FString& Response);

	UFUNCTION()
	void HandleLLMError(const FString& ErrorMessage);

public:	
	// Called every frame
	FORCEINLINE FString GetNPCName() const { return NPCName; }
	FORCEINLINE FString GetSystemPrompt() const { return SystemPrompt; }
};
