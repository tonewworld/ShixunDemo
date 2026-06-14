#include "MainMenuWidget.h"
#include "Kismet/GameplayStatics.h"

void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        PC->bShowMouseCursor = true;
        PC->SetInputMode(FInputModeUIOnly());
    }
}

void UMainMenuWidget::StartGame()
{
    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        PC->bShowMouseCursor = false;
        PC->SetInputMode(FInputModeGameOnly());
    }

    if (!TargetLevel.IsNone())
    {
        UGameplayStatics::OpenLevel(this, TargetLevel);
    }
}
