// PickupItem.cpp
#include "PickupItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

APickupItem::APickupItem()
{
	PrimaryActorTick.bCanEverTick = false;

	// Collision root — small sphere so line trace can hit it
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->SetSphereRadius(32.f);
	CollisionComp->SetCollisionProfileName(TEXT("BlockAll"));
	RootComponent = CollisionComp;

	// Visible mesh — assign in Blueprint
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APickupItem::BeginPlay()
{
	Super::BeginPlay();
}

void APickupItem::Interact_Implementation(APawn* InstigatorPawn)
{
	OnPickedUp(InstigatorPawn);
}

FText APickupItem::GetInteractionPrompt_Implementation() const
{
	return FText::FromString(FString::Printf(TEXT("Press E to pick up %s"), *ItemName));
}

void APickupItem::OnPickedUp_Implementation(APawn* InstigatorPawn)
{
	if (!InstigatorPawn)
	{
		return;
	}

	// Add the inventory tag to the player controller
	APlayerController* PC = Cast<APlayerController>(InstigatorPawn->GetController());
	if (PC)
	{
		// Store the item tag on the player controller using tags
		PC->Tags.AddUnique(InventoryTag);

		UE_LOG(LogTemp, Log, TEXT("Player picked up: %s (tag: %s)"),
			*ItemName, *InventoryTag.ToString());
	}

	// Destroy the pickup actor from the world
	Destroy();
}