// LLMConnector.cpp
#include "LLMConnector.h"
#include "DialogueWidget.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

ULLMConnector::ULLMConnector()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULLMConnector::SendConversation(const FString& SystemPrompt, UDialogueWidget* DialogueWidget)
{
	if (bIsWaitingForResponse)
	{
		UE_LOG(LogTemp, Warning, TEXT("LLMConnector: Already waiting for a response."));
		return;
	}

	if (!DialogueWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("LLMConnector: DialogueWidget is null!"));
		OnLLMError.Broadcast(TEXT("DialogueWidget is null"));
		return;
	}

	bIsWaitingForResponse = true;

	// Build the JSON request body
	FString RequestBody = BuildRequestJSON(SystemPrompt, DialogueWidget);

	UE_LOG(LogTemp, Log, TEXT("LLMConnector: Sending request to %s"), *ServerURL);
	UE_LOG(LogTemp, Verbose, TEXT("LLMConnector: Request body: %s"), *RequestBody);

	// Create the HTTP request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(ServerURL);
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetContentAsString(RequestBody);

	// Handle the response
	HttpRequest->OnProcessRequestComplete().BindLambda(
		[this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			bIsWaitingForResponse = false;

			if (!bConnectedSuccessfully || !Response.IsValid())
			{
				FString ErrorMsg = TEXT("Failed to connect to LLM server. Is llama-server running?");
				UE_LOG(LogTemp, Error, TEXT("LLMConnector: %s"), *ErrorMsg);
				OnLLMError.Broadcast(ErrorMsg);
				return;
			}

			int32 ResponseCode = Response->GetResponseCode();
			FString ResponseBody = Response->GetContentAsString();

			if (ResponseCode != 200)
			{
				FString ErrorMsg = FString::Printf(
					TEXT("LLM server returned HTTP %d: %s"), ResponseCode, *ResponseBody);
				UE_LOG(LogTemp, Error, TEXT("LLMConnector: %s"), *ErrorMsg);
				OnLLMError.Broadcast(ErrorMsg);
				return;
			}

			// Parse the assistant's message from the response
			FString AssistantMessage = ParseResponse(ResponseBody);

			if (AssistantMessage.IsEmpty())
			{
				OnLLMError.Broadcast(TEXT("Failed to parse LLM response"));
				return;
			}

			UE_LOG(LogTemp, Log, TEXT("LLMConnector: Received response: %s"),
				*AssistantMessage.Left(100));

			OnLLMResponse.Broadcast(AssistantMessage);
		}
	);

	HttpRequest->ProcessRequest();
}

FString ULLMConnector::BuildRequestJSON(const FString& SystemPrompt, UDialogueWidget* DialogueWidget) const
{
	// Build the messages array
	TSharedPtr<FJsonObject> RootObject = MakeShared<FJsonObject>();
	TArray<TSharedPtr<FJsonValue>> MessagesArray;

	// Add system prompt
	TSharedPtr<FJsonObject> SystemMsg = MakeShared<FJsonObject>();
	SystemMsg->SetStringField(TEXT("role"), TEXT("system"));
	SystemMsg->SetStringField(TEXT("content"), SystemPrompt);
	MessagesArray.Add(MakeShared<FJsonValueObject>(SystemMsg));

	// Add conversation history from the dialogue widget
	const TArray<FConversationMessage>& History = DialogueWidget->GetHistory();
	for (const FConversationMessage& Msg : History)
	{
		TSharedPtr<FJsonObject> HistoryMsg = MakeShared<FJsonObject>();
		HistoryMsg->SetStringField(TEXT("role"), Msg.Speaker);   // "user" or "assistant"
		HistoryMsg->SetStringField(TEXT("content"), Msg.Content);
		MessagesArray.Add(MakeShared<FJsonValueObject>(HistoryMsg));
	}

	RootObject->SetArrayField(TEXT("messages"), MessagesArray);
	RootObject->SetNumberField(TEXT("temperature"), Temperature);
	RootObject->SetNumberField(TEXT("max_tokens"), MaxTokens);
	RootObject->SetBoolField(TEXT("stream"), false);

	// Serialize to string
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);

	return OutputString;
}

FString ULLMConnector::ParseResponse(const FString& ResponseBody) const
{
	// Parse the OpenAI-compatible response format:
	// {
	//   "choices": [
	//     {
	//       "message": {
	//         "role": "assistant",
	//         "content": "Hello! I am Atlas..."
	//       }
	//     }
	//   ]
	// }

	TSharedPtr<FJsonObject> RootObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

	if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("LLMConnector: Failed to parse JSON response"));
		return FString();
	}

	const TArray<TSharedPtr<FJsonValue>>* ChoicesArray;
	if (!RootObject->TryGetArrayField(TEXT("choices"), ChoicesArray) || ChoicesArray->Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("LLMConnector: No 'choices' in response"));
		return FString();
	}

	TSharedPtr<FJsonObject> FirstChoice = (*ChoicesArray)[0]->AsObject();
	if (!FirstChoice.IsValid())
	{
		return FString();
	}

	const TSharedPtr<FJsonObject>* MessageObject;
	if (!FirstChoice->TryGetObjectField(TEXT("message"), MessageObject))
	{
		UE_LOG(LogTemp, Error, TEXT("LLMConnector: No 'message' in first choice"));
		return FString();
	}

	FString Content;
	if (!(*MessageObject)->TryGetStringField(TEXT("content"), Content))
	{
		UE_LOG(LogTemp, Error, TEXT("LLMConnector: No 'content' in message"));
		return FString();
	}

	return Content;
}