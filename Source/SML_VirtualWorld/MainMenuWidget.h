// MainMenuWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;
class UTextBlock;

/**
 * Main menu screen with Play and Quit buttons.
 *
 * Create a Widget Blueprint child of this class.
 * Add the following widgets with EXACT names:
 *
 *   - UButton    named  "Button_Play"
 *   - UButton    named  "Button_Quit"
 */
UCLASS()
class UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// ??? Widget bindings ??????????????????????????????????????

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Play;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Quit;

private:
	UFUNCTION()
	void OnPlayClicked();

	UFUNCTION()
	void OnQuitClicked();

	UFUNCTION()
	void OnPlayHovered();

	UFUNCTION()
	void OnPlayUnhovered();

	UFUNCTION()
	void OnQuitHovered();

	UFUNCTION()
	void OnQuitUnhovered();
};