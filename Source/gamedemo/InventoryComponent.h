#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemID.h"
#include "InventoryComponent.generated.h"

class UInventoryWidget;

USTRUCT(BlueprintType)
struct FSlotInfo
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EItemID ItemID = EItemID::None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FText ItemName;

    bool IsEmpty() const { return ItemID == EItemID::None; }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlaceItemFailed, EItemID, ItemID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnThreeKeysOffered);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GAMEDEMO_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    static const int32 InventorySize = 10;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TArray<FSlotInfo> Slots;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 AddItem(EItemID ItemID);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItem(int32 SlotIndex);

    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool HasItem(EItemID ItemID) const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 FindItem(EItemID ItemID) const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 GetCollectedKeyCount() const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool HasAllThreeKeys() const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool OfferThreeKeys();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void UseItemAtSlot(int32 SlotIndex);

    void ToggleInventory();

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventoryChanged OnInventoryChanged;

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnPlaceItemFailed OnPlaceItemFailed;

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnThreeKeysOffered OnThreeKeysOffered;

    UPROPERTY(VisibleAnywhere, Category = "Inventory")
    bool bInventoryOpen = false;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    static TSubclassOf<AActor> GetActorClassForItem(EItemID ItemID);

    UFUNCTION(BlueprintPure, Category = "Inventory")
    static FText GetItemDisplayName(EItemID ItemID);

    /** Widget class to use (set in BP). If null, uses the C++ default. */
    UPROPERTY(EditAnywhere, Category = "Inventory")
    TSubclassOf<UInventoryWidget> InventoryWidgetClass;

protected:
    UPROPERTY(EditAnywhere, Category = "Inventory")
    float PlaceDistance = 220.0f;

    UPROPERTY(EditAnywhere, Category = "Inventory")
    float PlaceTraceRadius = 45.0f;

private:
    UPROPERTY()
    UInventoryWidget* InventoryWidget;
};