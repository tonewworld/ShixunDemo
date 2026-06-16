#include "InventoryWidget.h"
#include "Components/GridSlot.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "Styling/CoreStyle.h"

static const int32 Cols = 5;
static const float SlotCellSize = 100.0f;

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!SlotGrid) return;

	SlotWidgets.SetNum(10);
	CurrentSlots.SetNum(10);

	for (int32 i = 0; i < 10; i++)
	{
		// Create slot border
		UBorder* SlotBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(),
			*FString::Printf(TEXT("Slot_%d"), i));
		SlotBorder->SetBrushColor(FLinearColor(0.2f, 0.2f, 0.25f, 1.0f));

		// Create slot text
		UTextBlock* TextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),
			*FString::Printf(TEXT("SlotText_%d"), i));
		TextBlock->SetText(FText::FromString(TEXT("空")));
		TextBlock->SetFont(FCoreStyle::GetDefaultFontStyle("Regular", 12));
		TextBlock->SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f));
		TextBlock->SetJustification(ETextJustify::Center);
		SlotBorder->SetContent(TextBlock);

		// Fixed size for each slot
		USizeBox* SizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(),
			*FString::Printf(TEXT("SizeBox_%d"), i));
		SizeBox->SetWidthOverride(SlotCellSize - 10.0f);
		SizeBox->SetHeightOverride(SlotCellSize - 10.0f);
		SizeBox->SetContent(SlotBorder);

		// Add to the BP's grid
		UGridSlot* GridSlotChild = SlotGrid->AddChildToGrid(SizeBox);
		GridSlotChild->SetRow(i / Cols);
		GridSlotChild->SetColumn(i % Cols);
		GridSlotChild->SetPadding(FMargin(5.0f));

		SlotWidgets[i] = {SlotBorder, TextBlock};
	}
}

FReply UInventoryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		FVector2D LocalPos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

		for (int32 i = 0; i < SlotWidgets.Num(); i++)
		{
			UBorder* Border = SlotWidgets[i].Border;
			if (!Border) continue;

			FGeometry SlotGeo = Border->GetCachedGeometry();
			FVector2D SlotAbsPos = SlotGeo.GetAbsolutePosition();
			FVector2D SlotAbsSize = SlotGeo.GetAbsoluteSize();
			FVector2D SlotLocalPos = InGeometry.AbsoluteToLocal(SlotAbsPos);

			if (LocalPos.X >= SlotLocalPos.X && LocalPos.X <= SlotLocalPos.X + SlotAbsSize.X &&
				LocalPos.Y >= SlotLocalPos.Y && LocalPos.Y <= SlotLocalPos.Y + SlotAbsSize.Y)
			{
				if (CurrentSlots.IsValidIndex(i) && !CurrentSlots[i].IsEmpty() && InventoryComp)
				{
					InventoryComp->UseItemAtSlot(i);
					UpdateSlots(InventoryComp->Slots);
					return FReply::Handled();
				}
			}
		}
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UInventoryWidget::UpdateSlots(const TArray<FSlotInfo>& InSlots)
{
	CurrentSlots = InSlots;
	for (int32 i = 0; i < FMath::Min(10, InSlots.Num()) && SlotWidgets.IsValidIndex(i); i++)
	{
		if (InSlots[i].IsEmpty())
		{
			SlotWidgets[i].Text->SetText(FText::FromString(TEXT("空")));
			SlotWidgets[i].Text->SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f));
			SlotWidgets[i].Border->SetBrushColor(FLinearColor(0.2f, 0.2f, 0.25f, 1.0f));
		}
		else
		{
			SlotWidgets[i].Text->SetText(InSlots[i].ItemName);
			SlotWidgets[i].Text->SetColorAndOpacity(FLinearColor::White);
			SlotWidgets[i].Border->SetBrushColor(FLinearColor(0.3f, 0.3f, 0.4f, 1.0f));
		}
	}
}
