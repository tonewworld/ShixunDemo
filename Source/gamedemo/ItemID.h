#pragma once

#include "CoreMinimal.h"
#include "ItemID.generated.h"

UENUM(BlueprintType)
enum class EItemID : uint8
{
	None		UMETA(DisplayName = "None"),
	Key			UMETA(DisplayName = "Key"),
	Ball		UMETA(DisplayName = "Ball"),
	HealthPotion UMETA(DisplayName = "Health Potion"),
};