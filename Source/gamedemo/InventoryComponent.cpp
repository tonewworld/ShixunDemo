#include "InventoryComponent.h"
#include "InventoryWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Slots.SetNum(InventorySize);
}

int32 UInventoryComponent::AddItem(EItemID ItemID)
{
	if (ItemID == EItemID::None) return INDEX_NONE;

	for (int32 i = 0; i < InventorySize; i++)
	{
		if (Slots[i].IsEmpty())
		{
			Slots[i].ItemID = ItemID;
			Slots[i].ItemName = GetItemDisplayName(ItemID);
			OnInventoryChanged.Broadcast();
			return i;
		}
	}
	return INDEX_NONE;
}

bool UInventoryComponent::RemoveItem(int32 SlotIndex)
{
	if (!Slots.IsValidIndex(SlotIndex) || Slots[SlotIndex].IsEmpty()) return false;

	Slots[SlotIndex] = FSlotInfo();
	OnInventoryChanged.Broadcast();
	return true;
}

bool UInventoryComponent::HasItem(EItemID ItemID) const
{
	return FindItem(ItemID) != INDEX_NONE;
}

int32 UInventoryComponent::FindItem(EItemID ItemID) const
{
	for (int32 i = 0; i < InventorySize; i++)
	{
		if (Slots[i].ItemID == ItemID)
			return i;
	}
	return INDEX_NONE;
}

void UInventoryComponent::UseItemAtSlot(int32 SlotIndex)
{
	if (!Slots.IsValidIndex(SlotIndex) || Slots[SlotIndex].IsEmpty()) return;

	ACharacter* Char = Cast<ACharacter>(GetOwner());
	if (!Char) return;

	APlayerController* PC = Cast<APlayerController>(Char->GetController());
	if (!PC) return;

	EItemID ItemID = Slots[SlotIndex].ItemID;

	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);

	FVector TraceEnd = CamLoc + CamRot.Vector() * PlaceDistance;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Char);

	FHitResult Hit;
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, TraceEnd, ECC_Visibility, Params);

	FVector PlaceLoc = bHit ? Hit.Location : TraceEnd;

	// Offset slightly back from hit surface so item doesn't clip in
	if (bHit)
	{
		PlaceLoc -= CamRot.Vector() * 20.0f;
	}

	// Check if location is clear via sphere overlap
	FCollisionShape CheckShape = FCollisionShape::MakeSphere(PlaceTraceRadius);
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams OverlapParams;
	OverlapParams.AddIgnoredActor(Char);

	GetWorld()->OverlapMultiByChannel(Overlaps, PlaceLoc, FQuat::Identity, ECC_WorldStatic, CheckShape, OverlapParams);

	if (Overlaps.Num() > 0)
	{
		OnPlaceItemFailed.Broadcast(ItemID);
		return;
	}

	// Spawn item at placement location
	TSubclassOf<AActor> ActorClass = GetActorClassForItem(ItemID);
	if (!ActorClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("No actor class found for ItemID %d"), (uint8)ItemID);
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ActorClass, PlaceLoc, FRotator::ZeroRotator, SpawnParams);

	if (Spawned)
	{
		RemoveItem(SlotIndex);
	}
}

void UInventoryComponent::ToggleInventory()
{
	if (!InventoryWidget)
	{
		TSubclassOf<UInventoryWidget> WidgetClass = InventoryWidgetClass
			? InventoryWidgetClass : UInventoryWidget::StaticClass();
		InventoryWidget = CreateWidget<UInventoryWidget>(GetWorld(), WidgetClass);
		if (InventoryWidget)
		{
			InventoryWidget->SetInventoryComponent(this);
		}
	}

	if (!InventoryWidget) return;

	ACharacter* Char = Cast<ACharacter>(GetOwner());
	APlayerController* PC = Char ? Cast<APlayerController>(Char->GetController()) : nullptr;

	if (bInventoryOpen)
	{
		InventoryWidget->RemoveFromViewport();
		bInventoryOpen = false;

		if (PC)
		{
			PC->SetInputMode(FInputModeGameOnly());
			PC->bShowMouseCursor = false;
			PC->bEnableClickEvents = true;
			PC->bEnableMouseOverEvents = true;
		}
	}
	else
	{
		InventoryWidget->AddToViewport();
		InventoryWidget->UpdateSlots(Slots);
		bInventoryOpen = true;

		if (PC)
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;
			PC->bEnableClickEvents = false;
			PC->bEnableMouseOverEvents = false;
		}
	}
}

TSubclassOf<AActor> UInventoryComponent::GetActorClassForItem(EItemID ItemID)
{
	switch (ItemID)
	{
	case EItemID::Key:
		return TSoftClassPtr<AActor>(FSoftObjectPath(TEXT("/Game/Assets/Blueprints/BP_Key.BP_Key_C"))).LoadSynchronous();
	case EItemID::Ball:
		return TSoftClassPtr<AActor>(FSoftObjectPath(TEXT("/Game/Assets/Blueprints/BP_Ball.BP_Ball_C"))).LoadSynchronous();
	default:
		return nullptr;
	}
}

FText UInventoryComponent::GetItemDisplayName(EItemID ItemID)
{
	switch (ItemID)
	{
	case EItemID::Key:	return NSLOCTEXT("Item", "Key", "钥匙");
	case EItemID::Ball:	return NSLOCTEXT("Item", "Ball", "球");
	default:			return FText::GetEmpty();
	}
}
