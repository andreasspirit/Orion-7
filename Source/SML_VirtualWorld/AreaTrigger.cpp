// Fill out your copyright notice in the Description page of Project Settings.


#include "AreaTrigger.h"
#include "GameStateManager.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/GameInstance.h"


// Sets default values
AAreaTrigger::AAreaTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->SetBoxExtent(FVector(300.0f, 300.0f, 150.0f));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerBox->SetHiddenInGame(true);
	TriggerBox->ShapeColor = FColor::Cyan;
}

// Called when the game starts or when spawned
void AAreaTrigger::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AAreaTrigger::OnTriggerBeginOverlap);

	// If the player spawns inside this box (the starting room), the overlap can happen
// before we start listening, so the event is missed. Check once on the next frame,
// when the player has been spawned and possessed.
	GetWorldTimerManager().SetTimerForNextTick(this, &AAreaTrigger::CheckInitialOverlaps);
	
}

void AAreaTrigger::CheckInitialOverlaps()
{
	TArray<AActor*> Overlapping;
	TriggerBox->GetOverlappingActors(Overlapping, APawn::StaticClass());
	for (AActor* Actor : Overlapping)
	{
		HandleActorEntered(Actor);
	}
}


void AAreaTrigger::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	HandleActorEntered(OtherActor);
}

void AAreaTrigger::HandleActorEntered(AActor* Actor)
{
	UE_LOG(LogTemp, Log, TEXT("AreaTrigger '%s': overlap with %s"), *AreaName, *GetNameSafe(Actor));
	// Check if the overlapping actor is the player pawn
	APawn* Pawn = Cast<APawn>(Actor);
	if(!Pawn || !Pawn->IsPlayerControlled())
	{
		return;
	}

	if(UGameInstance* GI = GetGameInstance())
	{
		if(UGameStateManager* State = GI->GetSubsystem<UGameStateManager>())
		{
			State->SetCurrentArea(AreaName, AreaDescription);
		}
	}
}



