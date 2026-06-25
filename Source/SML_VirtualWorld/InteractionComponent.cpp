// InteractionComponent.cpp
#include "InteractionComponent.h"
#include "InteractableInterface.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Camera/PlayerCameraManager.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	SetupInput();
}

void UInteractionComponent::SetupInput()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractionComponent: Owner is not a Pawn!"));
		return;
	}

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC)
	{
		return;
	}

	// Add the mapping context if one is assigned
	if (InteractionMappingContext)
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());

		if (InputSubsystem)
		{
			InputSubsystem->AddMappingContext(InteractionMappingContext, MappingContextPriority);
		}
	}

	// Bind the interact action
	if (InteractAction)
	{
		UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(OwnerPawn->InputComponent);
		if (EnhancedInput)
		{
			EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started, this, &UInteractionComponent::OnInteract);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractionComponent: No InteractAction assigned! Set it in the component details."));
	}
}

void UInteractionComponent::OnInteract(const FInputActionValue& Value)
{
	AActor* HitActor = TraceForInteractable();

	if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		APawn* OwnerPawn = Cast<APawn>(GetOwner());
		IInteractableInterface::Execute_Interact(HitActor, OwnerPawn);

		UE_LOG(LogTemp, Log, TEXT("InteractionComponent: Interacted with %s"), *HitActor->GetName());
	}
}

AActor* UInteractionComponent::TraceForInteractable() const
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return nullptr;
	}

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC || !PC->PlayerCameraManager)
	{
		return nullptr;
	}

	// Trace from the camera forward
	FVector Start = PC->PlayerCameraManager->GetCameraLocation();
	FVector Direction = PC->PlayerCameraManager->GetCameraRotation().Vector();
	FVector End = Start + (Direction * InteractionRange);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerPawn);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	if (bHit)
	{
		return HitResult.GetActor();
	}

	return nullptr;
}