// AccessCardSpawner.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AccessCardSpawner.generated.h"

class APickupItem;

/**
 * Place this actor in your level.
 * Add child Scene Components as spawn points — the access card
 * will randomly spawn at one of them on BeginPlay.
 */
UCLASS()
class AAccessCardSpawner : public AActor
{
	GENERATED_BODY()

public:
	AAccessCardSpawner();

protected:
	virtual void BeginPlay() override;

	// ??? Configuration ????????????????????????????????????????

	/** The access card Blueprint class to spawn. Set this to BP_AccessCard. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	TSubclassOf<APickupItem> AccessCardClass;

	/** List of possible spawn locations (world space).
	 *  Add as many as you want — one will be picked randomly. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	TArray<FVector> SpawnLocations;

	/** Optional: randomize rotation too. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	bool bRandomizeRotation = false;

private:
	/** Picks a random location and spawns the card there. */
	void SpawnAccessCard();
};