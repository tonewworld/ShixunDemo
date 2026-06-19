#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OfferingWidget.generated.h"

class UInventoryComponent;
class UTextBlock;
class UButton;
class UVerticalBox;

UCLASS()
class GAMEDEMO_API UOfferingWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void Init(UInventoryComponent* InInventory);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOfferComplete);
    UPROPERTY(BlueprintAssignable, Category = "Offering")
    FOnOfferComplete OnOfferComplete;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UFUNCTION()
    void OnOfferClicked();

private:
    UPROPERTY()
    UInventoryComponent* Inventory;

    UPROPERTY()
    UTextBlock* ProgressText;

    UPROPERTY()
    UTextBlock* HintText;

    UPROPERTY()
    UButton* OfferButton;

    UPROPERTY()
    UTextBlock* OfferButtonText;

    void RefreshUI();
};