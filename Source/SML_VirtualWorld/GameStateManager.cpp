// GameStateManager.cpp
#include "GameStateManager.h"
#include "Engine/Engine.h"

void UGameStateManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ResetState();
	UE_LOG(LogTemp, Log, TEXT("GameStateManager initialised"));
}

void UGameStateManager::SetCurrentArea(const FString& AreaName, const FString& AreaDescription)
{
	if (AreaName == CurrentArea)
	{
		return; // already here, nothing changed
	}

	CurrentArea = AreaName;
	CurrentAreaDescription = AreaDescription;
	AreaEnterTime = FPlatformTime::Seconds();

	LogEvent(FString::Printf(TEXT("Player entered %s"), *AreaName));
	ShowDebug(FString::Printf(TEXT("Area: %s"), *AreaName));
}

void UGameStateManager::CompleteObjective(const FString& Objective)
{
	if (!CompletedObjectives.Contains(Objective))
	{
		CompletedObjectives.Add(Objective);
		LogEvent(FString::Printf(TEXT("Objective completed: %s"), *Objective));
		ShowDebug(FString::Printf(TEXT("Objective: %s"), *Objective));
	}
}

void UGameStateManager::AddItem(const FString& ItemName)
{
	if (!Inventory.Contains(ItemName))
	{
		Inventory.Add(ItemName);
		LogEvent(FString::Printf(TEXT("Player picked up %s"), *ItemName));
		ShowDebug(FString::Printf(TEXT("Item: %s"), *ItemName));
	}
}

bool UGameStateManager::HasItem(const FString& ItemName) const
{
	return Inventory.Contains(ItemName);
}

void UGameStateManager::SetThreatNearby(bool bNearby)
{
	if (bThreatNearby == bNearby)
	{
		return;
	}
	bThreatNearby = bNearby;
	LogEvent(bNearby ? TEXT("An alien is close to the player") : TEXT("The alien moved away"));
}

void UGameStateManager::LogEvent(const FString& Event)
{
	RecentEvents.Add(Event);
	if (RecentEvents.Num() > MaxRecentEvents)
	{
		RecentEvents.RemoveAt(0); // drop the oldest
	}
	UE_LOG(LogTemp, Log, TEXT("GameState event: %s"), *Event);
}

void UGameStateManager::ResetState()
{
	CurrentArea = TEXT("Unknown");
	CurrentAreaDescription.Empty();
	AreaEnterTime = FPlatformTime::Seconds();
	CompletedObjectives.Empty();
	Inventory.Empty();
	RecentEvents.Empty();
	bThreatNearby = false;
}

FString UGameStateManager::BuildContextString() const
{
	auto JoinOrNone = [](const TArray<FString>& List)
		{
			return List.Num() > 0 ? FString::Join(List, TEXT(", ")) : FString(TEXT("none"));
		};

	const int32 SecondsInArea = FMath::RoundToInt(FPlatformTime::Seconds() - AreaEnterTime);

	FString Context = TEXT("CURRENT GAME STATE:\n");
	Context += FString::Printf(TEXT("- Player location: %s"), *CurrentArea);
	if (!CurrentAreaDescription.IsEmpty())
	{
		Context += FString::Printf(TEXT(" (%s)"), *CurrentAreaDescription);
	}
	Context += TEXT("\n");
	Context += FString::Printf(TEXT("- Time in this area: %d seconds\n"), SecondsInArea);
	Context += FString::Printf(TEXT("- Items carried: %s\n"), *JoinOrNone(Inventory));
	Context += FString::Printf(TEXT("- Completed objectives: %s\n"), *JoinOrNone(CompletedObjectives));
	Context += FString::Printf(TEXT("- Alien nearby: %s\n"), bThreatNearby ? TEXT("YES") : TEXT("no"));
	Context += FString::Printf(TEXT("- Recent events: %s\n"), *JoinOrNone(RecentEvents));
	return Context;
}

void UGameStateManager::ShowDebug(const FString& Message) const
{
	// On-screen message for testing. Remove before the evaluation build.
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Cyan, Message);
	}
}