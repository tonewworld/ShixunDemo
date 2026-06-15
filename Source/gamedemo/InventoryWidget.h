#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/GridPanel.h"
#include "InventoryComponent.h"
#include "InventoryWidget.generated.h"

UCLASS()
class GAMEDEMO_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void UpdateSlots(const TArray<FSlotInfo>& InSlots);
	void SetInventoryComponent(UInventoryComponent* InComp) { InventoryComp = InComp; }

protected:
	/** GridPanel named "SlotGrid" in the WBP. C++ fills it with 10 slot children. */
	UPROPERTY(meta = (BindWidget))
	UGridPanel* SlotGrid;

private:
	struct FSlotWidgets
	{
		class UBorder* Border;
		class UTextBlock* Text;
	};

	TArray<FSlotWidgets> SlotWidgets;
	TArray<FSlotInfo> CurrentSlots;

	UPROPERTY()
	UInventoryComponent* InventoryComp;
};
