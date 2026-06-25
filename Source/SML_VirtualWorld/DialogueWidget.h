// DialogueWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueWidget.generated.h"

class UScrollBox;
class UEditableTextBox;
class UButton;
class UTextBlock;

/** A single message in the conversation. */
USTRUCT(BlueprintType)
struct FConversationMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Speaker;

	UPROPERTY(BlueprintReadWrite)
	FString Content;

	UPROPERTY(BlueprintReadWrite)
	bool bIsPlayer = false;
};

/**
 * The dialogue UI that appears when talking to an NPC.
 * Displays a scrollable message list with a text input at the bottom.
 *
 * IMPORTANT: After creating this C++ class, create a Widget Blueprint child of it.
 * In that Blueprint, add the following widgets and name them EXACTLY as shown:
 *
 *   - UTextBlock        named  "Text_NPCName"
 *   - UScrollBox        named  "ScrollBox_Messages"
 *   - UEditableTextBox  named  "TextBox_Input"
 *   - UButton           named  "Button_Send"
 *   - UButton           named  "Button_Close"
 *   - UTextBlock        named  "Text_Thinking"    (set to Hidden by default)
 */
UCLASS()
class UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// ─── Initialization ───────────────────────────────────────

	/** Call this right after creating the widget to set the NPC info. */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void InitDialogue(const FString& InNPCName, const FString& InSystemPrompt);

	// ─── Message handling ─────────────────────────────────────

	/** Adds a player message to the scroll box and triggers the send flow. */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SendPlayerMessage();

	/** Called when the NPC response arrives from the LLM. */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void ReceiveNPCMessage(const FString& Message);

	/** Adds a message entry to the scroll box. */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void AddMessageToUI(const FString& Speaker, const FString& Content, bool bIsPlayer);

	/** Shows or hides the "thinking..." indicator. */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetThinkingVisible(bool bVisible);

	// ─── Event dispatchers (bind to these from the NPC) ───────

	/** Fires when the player sends a message. The NPC listens to this
	 *  and forwards the full history to the LLM connector. */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerMessageSent, const FString&, Message);

	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FOnPlayerMessageSent OnPlayerMessageSent;

	/** Fires when the player closes the dialogue. */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueClosed);

	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FOnDialogueClosed OnDialogueClosed;

protected:
	virtual void NativeConstruct() override;

	// ─── Widget bindings (matched by name in the UMG designer) ─

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_NPCName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_Messages;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_Input;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Send;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Close;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Thinking;

	// ─── Internal state ───────────────────────────────────────

	UPROPERTY()
	FString NPCName;

	UPROPERTY()
	FString SystemPrompt;

	UPROPERTY()
	TArray<FConversationMessage> ConversationHistory;

	// ─── Button callbacks ─────────────────────────────────────

	UFUNCTION()
	void OnSendClicked();

	UFUNCTION()
	void OnCloseClicked();

	UFUNCTION()
	void OnInputCommitted(const FText& Text, ETextCommit::Type CommitMethod);

public:
	/** Returns the full conversation history (for sending to the LLM). */
	FORCEINLINE const TArray<FConversationMessage>& GetHistory() const { return ConversationHistory; }
	FORCEINLINE FString GetSystemPrompt() const { return SystemPrompt; }
};