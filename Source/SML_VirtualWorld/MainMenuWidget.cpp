// MainMenuWidget.cpp
#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Play)
	{
		Button_Play->OnClicked.AddDynamic(this, &UMainMenuWidget::OnPlayClicked);
		Button_Play->OnHovered.AddDynamic(this, &UMainMenuWidget::OnPlayHovered);
		Button_Play->OnUnhovered.AddDynamic(this, &UMainMenuWidget::OnPlayUnhovered);
	}

	if (Button_Quit)
	{
		Button_Quit->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);
		Button_Quit->OnHovered.AddDynamic(this, &UMainMenuWidget::OnQuitHovered);
		Button_Quit->OnUnhovered.AddDynamic(this, &UMainMenuWidget::OnQuitUnhovered);
	}
}

void UMainMenuWidget::OnPlayClicked()
{
	// Open your game level — change this to your actual level name
	UGameplayStatics::OpenLevel(this, FName("FirstPersonMap"));
}

void UMainMenuWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(
		this,
		nullptr,
		EQuitPreference::Quit,
		true
	);
}

void UMainMenuWidget::OnPlayHovered()
{
	if (Button_Play)
	{
		Button_Play->SetRenderScale(FVector2D(1.05f, 1.05f));
	}
}

void UMainMenuWidget::OnPlayUnhovered()
{
	if (Button_Play)
	{
		Button_Play->SetRenderScale(FVector2D(1.0f, 1.0f));
	}
}

void UMainMenuWidget::OnQuitHovered()
{
	if (Button_Quit)
	{
		Button_Quit->SetRenderScale(FVector2D(1.05f, 1.05f));
	}
}

void UMainMenuWidget::OnQuitUnhovered()
{
	if (Button_Quit)
	{
		Button_Quit->SetRenderScale(FVector2D(1.0f, 1.0f));
	}
}