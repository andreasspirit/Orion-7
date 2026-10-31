// GameStateManager.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameStateManager.generated.h"

/**
 * Tracks what is happening in the game so the AI companion can be told about it.
 * One instance exists for the whole game (created automatically by Unreal).
 *
 * Access from any actor:
 *   UGameStateManager* State = GetGameInstance()->GetSubsystem<UGameStateManager>();
 */
UCLASS()
class UGameStateManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Called by area triggers when the player enters a room. */
	UFUNCTION(BlueprintCallable, Category = "Game State")
	void SetCurrentArea(const FString& AreaName, const FString& AreaDescription);

	/** Mark an objective as done, e.g. "Unlocked the shelter door". */
	UFUNCTION(BlueprintCallable, Category = "Game State")
	void CompleteObjective(const FString& Objective);

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void AddItem(const FString& ItemName);

	UFUNCTION(BlueprintPure, Category = "Game State")
	bool HasItem(const FString& ItemName) const;

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void SetThreatNearby(bool bNearby);

	/** Record something that just happened, e.g. "Player picked up the access card". */
	UFUNCTION(BlueprintCallable, Category = "Game State")
	void LogEvent(const FString& Event);

	/** Clears everything. Call when a new game starts. */
	UFUNCTION(BlueprintCallable, Category = "Game State")
	void ResetState();

	/** Builds the text block that gets added to the companion's system prompt. */
	UFUNCTION(BlueprintPure, Category = "Game State")
	FString BuildContextString() const;

	UFUNCTION(BlueprintPure, Category = "Game State")
	FString GetCurrentArea() const { return CurrentArea; }

private:
	FString CurrentArea;
	FString CurrentAreaDescription;
	double AreaEnterTime = 0.0;

	TArray<FString> CompletedObjectives;
	TArray<FString> Inventory;
	TArray<FString> RecentEvents;
	bool bThreatNearby = false;

	/** Only the latest events are kept so the prompt stays short (faster responses). */
	static constexpr int32 MaxRecentEvents = 5;

	void ShowDebug(const FString& Message) const;
};