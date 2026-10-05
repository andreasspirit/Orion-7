// AccessCardSpawner.cpp
#include "AccessCardSpawner.h"
#include "PickupItem.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"

AAccessCardSpawner::AAccessCardSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AAccessCardSpawner::BeginPlay()
{
	Super::BeginPlay();
	SpawnAccessCard();
}

void AAccessCardSpawner::SpawnAccessCard()
{
	if (!AccessCardClass)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("AccessCardSpawner: No AccessCardClass assigned!"));
		return;
	}

	if (SpawnLocations.Num() == 0)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("AccessCardSpawner: No spawn locations defined!"));
		return;
	}

	// Pick a random location from the array
	int32 RandomIndex = FMath::RandRange(0, SpawnLocations.Num() - 1);
	FVector ChosenLocation = SpawnLocations[RandomIndex];

	// Build the spawn transform
	FRotator SpawnRotation = FRotator::ZeroRotator;
	if (bRandomizeRotation)
	{
		SpawnRotation = FRotator(0.f, FMath::RandRange(0.f, 360.f), 0.f);
	}

	FTransform SpawnTransform(SpawnRotation, ChosenLocation);

	// Spawn the card
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APickupItem* SpawnedCard = GetWorld()->SpawnActor<APickupItem>(
		AccessCardClass,
		SpawnTransform,
		SpawnParams
	);

	if (SpawnedCard)
	{
		UE_LOG(LogTemp, Log,
			TEXT("AccessCardSpawner: Card spawned at location %d: %s"),
			RandomIndex, *ChosenLocation.ToString());
	}
}