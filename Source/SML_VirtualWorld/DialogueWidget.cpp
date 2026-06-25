// DialogueWidget.cpp
#include "DialogueWidget.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button clicks
	if (Button_Send)
	{
		Button_Send->OnClicked.AddDynamic(this, &UDialogueWidget::OnSendClicked);
	}

	if (Button_Close)
	{
		Button_Close->OnClicked.AddDynamic(this, &UDialogueWidget::OnCloseClicked);
	}

	// Bind Enter key on the text input
	if (EditableTextBox_Input)
	{
		EditableTextBox_Input->OnTextCommitted.AddDynamic(this, &UDialogueWidget::OnInputCommitted);
	}

	// Hide the thinking indicator by default
	SetThinkingVisible(false);
}

// ─────────────────────────────────────────────────────────────
// Initialization
// ─────────────────────────────────────────────────────────────

void UDialogueWidget::InitDialogue(const FString& InNPCName, const FString& InSystemPrompt)
{
	NPCName = InNPCName;
	SystemPrompt = InSystemPrompt;

	if (Text_NPCName)
	{
		Text_NPCName->SetText(FText::FromString(NPCName));
	}

	// Clear any leftover messages from a previous conversation
	ConversationHistory.Empty();
	if (ScrollBox_Messages)
	{
		ScrollBox_Messages->ClearChildren();
	}
}

// ─────────────────────────────────────────────────────────────
// Message handling
// ─────────────────────────────────────────────────────────────

void UDialogueWidget::SendPlayerMessage()
{
	if (!EditableTextBox_Input)
	{
		return;
	}

	FString PlayerText = EditableTextBox_Input->GetText().ToString().TrimStartAndEnd();
	if (PlayerText.IsEmpty())
	{
		return;
	}

	// Add to UI and history
	AddMessageToUI(TEXT("You"), PlayerText, true);

	FConversationMessage Msg;
	Msg.Speaker = TEXT("user");
	Msg.Content = PlayerText;
	Msg.bIsPlayer = true;
	ConversationHistory.Add(Msg);

	// Clear the input field
	EditableTextBox_Input->SetText(FText::GetEmpty());

	// Show thinking indicator
	SetThinkingVisible(true);

	// Broadcast so the NPC / LLM connector knows to send the request
	OnPlayerMessageSent.Broadcast(PlayerText);
}

void UDialogueWidget::ReceiveNPCMessage(const FString& Message)
{
	// Hide thinking indicator
	SetThinkingVisible(false);

	// Add to UI and history
	AddMessageToUI(NPCName, Message, false);

	FConversationMessage Msg;
	Msg.Speaker = TEXT("assistant");
	Msg.Content = Message;
	Msg.bIsPlayer = false;
	ConversationHistory.Add(Msg);
}

void UDialogueWidget::AddMessageToUI(const FString& Speaker, const FString& Content, bool bIsPlayer)
{
	if (!ScrollBox_Messages)
	{
		return;
	}

	// Create a text block for this message
	UTextBlock* MessageText = NewObject<UTextBlock>(this);
	if (!MessageText)
	{
		return;
	}

	// Format: "Speaker: Content"
	FString FormattedMessage = FString::Printf(TEXT("%s: %s"), *Speaker, *Content);
	MessageText->SetText(FText::FromString(FormattedMessage));
	MessageText->SetAutoWrapText(true);

	// Color: white for player, light green for NPC
	if (bIsPlayer)
	{
		MessageText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	}
	else
	{
		MessageText->SetColorAndOpacity(FSlateColor(FLinearColor(0.4f, 1.0f, 0.6f)));
	}

	// Add to scroll box and scroll to bottom
	ScrollBox_Messages->AddChild(MessageText);
	ScrollBox_Messages->ScrollToEnd();
}

void UDialogueWidget::SetThinkingVisible(bool bVisible)
{
	if (Text_Thinking)
	{
		Text_Thinking->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

// ─────────────────────────────────────────────────────────────
// Button callbacks
// ─────────────────────────────────────────────────────────────

void UDialogueWidget::OnSendClicked()
{
	SendPlayerMessage();
}

void UDialogueWidget::OnCloseClicked()
{
	OnDialogueClosed.Broadcast();
}

void UDialogueWidget::OnInputCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	// Only send on Enter key, not on focus loss
	if (CommitMethod == ETextCommit::OnEnter)
	{
		SendPlayerMessage();
	}
}