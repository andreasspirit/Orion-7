// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AreaTrigger.generated.h"

class UBoxComponent;

/**
 * An invisible trigger box that detects when the player enters a room.
 * When the player enters, it tells the GameStateManager what area they are in.
 */
UCLASS()
class SML_VIRTUALWORLD_API AAreaTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAreaTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> TriggerBox;	

	/** The name of the area this trigger represents. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area")
	FString AreaName = TEXT("Unnamed Area");

	/** A description of the area, for the AI companion. */ 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area", meta = (MultiLine = true))
	FString AreaDescription;

	UFUNCTION()
    void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult); 

private:
	/** Handles the player starting the game already inside this box. */
	void CheckInitialOverlaps();

	/** Shared logic for "an actor is now inside this area". */
	void HandleActorEntered(AActor* Actor);
};
