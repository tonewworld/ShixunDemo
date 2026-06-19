#include "OfferingWidget.h"
#include "InventoryComponent.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Blueprint/WidgetTree.h"

void UOfferingWidget::Init(UInventoryComponent* InInventory)
{
    Inventory = InInventory;

    // --- 构建 UI ---
    UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
    WidgetTree->RootWidget = Canvas;

    UBorder* Background = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Background"));
    Background->SetBrushColor(FLinearColor(0, 0, 0, 0.6f));
    UCanvasPanelSlot* BgSlot = Canvas->AddChildToCanvas(Background);
    BgSlot->SetAnchors(FAnchors(0, 0, 1, 1));
    BgSlot->SetOffsets(FMargin(0));

    UVerticalBox* VBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VBox"));
    UCanvasPanelSlot* VBoxSlot = Canvas->AddChildToCanvas(VBox);
    VBoxSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
    VBoxSlot->SetAlignment(FVector2D(0.5f, 0.5f));
    VBoxSlot->SetAutoSize(true);

    UTextBlock* TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Title"));
    TitleText->SetText(FText::FromString(TEXT("钥匙收集进度")));
    TitleText->SetJustification(ETextJustify::Center);
    FSlateFontInfo TitleFont = TitleText->Font;
    TitleFont.Size = 28;
    TitleText->SetFont(TitleFont);
    UVerticalBoxSlot* TitleSlot = VBox->AddChildToVerticalBox(TitleText);
    TitleSlot->SetPadding(FMargin(0, 0, 0, 16));

    ProgressText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Progress"));
    ProgressText->SetJustification(ETextJustify::Center);
    FSlateFontInfo ProgressFont = ProgressText->Font;
    ProgressFont.Size = 48;
    ProgressText->SetFont(ProgressFont);
    ProgressText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.84f, 0.0f)));
    UVerticalBoxSlot* ProgSlot = VBox->AddChildToVerticalBox(ProgressText);
    ProgSlot->SetPadding(FMargin(0, 0, 0, 12));

    HintText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Hint"));
    HintText->SetJustification(ETextJustify::Center);
    FSlateFontInfo HintFont = HintText->Font;
    HintFont.Size = 20;
    HintText->SetFont(HintFont);
    HintText->SetColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)));
    UVerticalBoxSlot* HintSlot = VBox->AddChildToVerticalBox(HintText);
    HintSlot->SetPadding(FMargin(0, 0, 0, 24));

    OfferButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("OfferButton"));
    OfferButton->SetVisibility(ESlateVisibility::Collapsed);
    UVerticalBoxSlot* BtnSlot = VBox->AddChildToVerticalBox(OfferButton);
    BtnSlot->SetPadding(FMargin(80, 0, 80, 0));

    OfferButtonText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("OfferButtonText"));
    OfferButtonText->SetText(FText::FromString(TEXT("供奉三把钥匙")));
    OfferButtonText->SetJustification(ETextJustify::Center);
    FSlateFontInfo BtnFont = OfferButtonText->Font;
    BtnFont.Size = 24;
    OfferButtonText->SetFont(BtnFont);
    OfferButton->AddChild(OfferButtonText);

    OfferButton->OnClicked.AddDynamic(this, &UOfferingWidget::OnOfferClicked);

    RefreshUI();
}

void UOfferingWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UOfferingWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    RefreshUI();
}

void UOfferingWidget::RefreshUI()
{
    if (!Inventory) return;

    int32 Collected = Inventory->GetCollectedKeyCount();
    bool bAll = Collected == 3;

    if (ProgressText)
    {
        ProgressText->SetText(FText::FromString(FString::Printf(TEXT("%d / 3"), Collected)));
    }

    if (HintText)
    {
        if (bAll)
        {
            HintText->SetText(FText::FromString(TEXT("已集齐三把钥匙，请供奉以开启通路")));
            HintText->SetColorAndOpacity(FSlateColor(FLinearColor(0.3f, 1.0f, 0.3f)));
        }
        else
        {
            HintText->SetText(FText::FromString(FString::Printf(TEXT("还需收集 %d 把钥匙"), 3 - Collected)));
            HintText->SetColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)));
        }
    }

    if (OfferButton)
    {
        OfferButton->SetVisibility(bAll ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void UOfferingWidget::OnOfferClicked()
{
    if (!Inventory) return;
    if (Inventory->OfferThreeKeys())
    {
        OfferButton->SetVisibility(ESlateVisibility::Collapsed);
        OnOfferComplete.Broadcast();
    }
}