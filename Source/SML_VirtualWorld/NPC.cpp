// ConversationalNPC.cpp
#include "NPC.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "DialogueWidget.h"
#include "LLMConnector.h"

ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = false;

	// ─── Root: Static mesh for the NPC body ───────────────────
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetCollisionProfileName(TEXT("BlockAll"));

	// ─── Interaction sphere (trigger volume) ──────────────────
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetSphereRadius(InteractionRadius);
	InteractionSphere->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	InteractionSphere->SetGenerateOverlapEvents(true);

	// ─── Overhead widget (optional floating name / prompt) ────
	OverheadWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidgetComp->SetupAttachment(RootComponent);
	OverheadWidgetComp->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	OverheadWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	OverheadWidgetComp->SetDrawAtDesiredSize(true);

	LLMConnector = CreateDefaultSubobject<ULLMConnector>(TEXT("LLMConnector"));
}

void ANPC::BeginPlay()
{
	Super::BeginPlay();

	// Update sphere radius in case it was changed in the editor
	InteractionSphere->SetSphereRadius(InteractionRadius);

	// Bind overlap events
	InteractionSphere->OnComponentBeginOverlap.AddDynamic(
		this, &ANPC::OnInteractionSphereBeginOverlap);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(
		this, &ANPC::OnInteractionSphereEndOverlap);
}

// ─────────────────────────────────────────────────────────────
// Overlap events
// ─────────────────────────────────────────────────────────────

void ANPC::OnInteractionSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn || !Pawn->IsLocallyControlled())
	{
		return;
	}

	NearbyPawn = Pawn;

	// TODO: Show "Press E to talk" prompt
	// You can do this by toggling visibility on the OverheadWidgetComp,
	// or by notifying the player's HUD.
	UE_LOG(LogTemp, Log, TEXT("[%s] Player entered interaction range"), *NPCName);
}

void ANPC::OnInteractionSphereEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (Pawn && Pawn == NearbyPawn)
	{
		NearbyPawn = nullptr;

		// If the player walks away mid-conversation, close the dialogue
		if (bIsInConversation)
		{
			EndConversation();
		}

		UE_LOG(LogTemp, Log, TEXT("[%s] Player left interaction range"), *NPCName);
	}
}

// ─────────────────────────────────────────────────────────────
// Interactable interface
// ─────────────────────────────────────────────────────────────

void ANPC::Interact_Implementation(APawn* InstigatorPawn)
{
	if (bIsInConversation)
	{
		EndConversation();
	}
	else
	{
		StartConversation(InstigatorPawn);
	}
}

FText ANPC::GetInteractionPrompt_Implementation() const
{
	if (bIsInConversation)
	{
		return FText::FromString(FString::Printf(TEXT("Press E to end conversation with %s"), *NPCName));
	}
	return FText::FromString(FString::Printf(TEXT("Press E to talk to %s"), *NPCName));
}

// ─────────────────────────────────────────────────────────────
// Dialogue lifecycle
// ─────────────────────────────────────────────────────────────

void ANPC::StartConversation(APawn* InstigatorPawn)
{
	if (bIsInConversation || !DialogueWidgetClass)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(InstigatorPawn->GetController());
	if (!PC)
	{
		return;
	}

	// Create and display the dialogue widget
	ActiveDialogueWidget = CreateWidget<UUserWidget>(PC, DialogueWidgetClass);
	if (!ActiveDialogueWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Failed to create dialogue widget!"), *NPCName);
		return;
	}

	ActiveDialogueWidget->AddToViewport(10);
	bIsInConversation = true;

	// Switch to UI + Game input so the player can type but also move the camera
	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(ActiveDialogueWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;

	UE_LOG(LogTemp, Log, TEXT("[%s] Conversation started"), *NPCName);

	// Wire up the dialogue widget
	UDialogueWidget* DialogueUI = Cast<UDialogueWidget>(ActiveDialogueWidget);
	if (DialogueUI)
	{
		DialogueUI->InitDialogue(NPCName, SystemPrompt);
		DialogueUI->OnDialogueClosed.AddDynamic(this, &ANPC::EndConversation);
	}

	DialogueUI->OnPlayerMessageSent.AddDynamic(this, &ANPC::HandlePlayerMessage);
	LLMConnector->OnLLMResponse.AddDynamic(this, &ANPC::HandleLLMResponse);
	LLMConnector->OnLLMError.AddDynamic(this, &ANPC::HandleLLMError);
}

void ANPC::EndConversation()
{
	if (!bIsInConversation)
	{
		return;
	}

	// Remove the dialogue widget
	if (ActiveDialogueWidget)
	{
		ActiveDialogueWidget->RemoveFromParent();
		ActiveDialogueWidget = nullptr;
	}

	bIsInConversation = false;

	// Restore game-only input
	if (NearbyPawn)
	{
		APlayerController* PC = Cast<APlayerController>(NearbyPawn->GetController());
		if (PC)
		{
			PC->SetInputMode(FInputModeGameOnly());
			PC->bShowMouseCursor = false;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[%s] Conversation ended"), *NPCName);
}



// In the .cpp:
void ANPC::HandlePlayerMessage(const FString& Message)
{
	UDialogueWidget* DialogueUI = Cast<UDialogueWidget>(ActiveDialogueWidget);
	if (DialogueUI && LLMConnector)
	{
		LLMConnector->SendConversation(SystemPrompt, DialogueUI);
	}
}

void ANPC::HandleLLMResponse(const FString& Response)
{
	UDialogueWidget* DialogueUI = Cast<UDialogueWidget>(ActiveDialogueWidget);
	if (DialogueUI)
	{
		DialogueUI->ReceiveNPCMessage(Response);
	}
}

void ANPC::HandleLLMError(const FString& ErrorMessage)
{
	UDialogueWidget* DialogueUI = Cast<UDialogueWidget>(ActiveDialogueWidget);
	if (DialogueUI)
	{
		DialogueUI->ReceiveNPCMessage(FString::Printf(TEXT("[Error: %s]"), *ErrorMessage));
	}
}