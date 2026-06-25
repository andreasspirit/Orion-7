// LLMConnector.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LLMConnector.generated.h"

class UDialogueWidget;

/** Fired when the LLM responds. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLLMResponse, const FString&, Response);

/** Fired when the LLM request fails. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLLMError, const FString&, ErrorMessage);

/**
 * Sends conversation history to a local llama.cpp server
 * and returns the assistant's response.
 *
 * Attach this component to your ConversationalNPC.
 * It uses the OpenAI-compatible /v1/chat/completions endpoint.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ULLMConnector : public UActorComponent
{
	GENERATED_BODY()

public:
	ULLMConnector();

	// ??? Configuration ????????????????????????????????????????

	/** The llama.cpp server URL. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LLM")
	FString ServerURL = TEXT("http://127.0.0.1:8080/v1/chat/completions");

	/** Temperature for generation (0.0 = deterministic, 1.0 = creative). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LLM", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float Temperature = 0.7f;

	/** Max tokens to generate in the response. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LLM", meta = (ClampMin = "1", ClampMax = "4096"))
	int32 MaxTokens = 256;

	// ??? Events ???????????????????????????????????????????????

	UPROPERTY(BlueprintAssignable, Category = "LLM")
	FOnLLMResponse OnLLMResponse;

	UPROPERTY(BlueprintAssignable, Category = "LLM")
	FOnLLMError OnLLMError;

	// ??? API ??????????????????????????????????????????????????

	/**
	 * Sends the full conversation to the LLM server.
	 * @param SystemPrompt  The NPC personality prompt.
	 * @param DialogueWidget  The widget holding conversation history.
	 */
	UFUNCTION(BlueprintCallable, Category = "LLM")
	void SendConversation(const FString& SystemPrompt, UDialogueWidget* DialogueWidget);

private:
	/** Builds the JSON body for the OpenAI-compatible API. */
	FString BuildRequestJSON(const FString& SystemPrompt, UDialogueWidget* DialogueWidget) const;

	/** Parses the assistant message from the JSON response. */
	FString ParseResponse(const FString& ResponseBody) const;

	/** True while waiting for a response. */
	bool bIsWaitingForResponse = false;
};