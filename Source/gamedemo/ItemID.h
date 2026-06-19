#pragma once

#include "CoreMinimal.h"
#include "ItemID.generated.h"

UENUM(BlueprintType)
enum class EItemID : uint8
{
    None         UMETA(DisplayName = "None"),
    Key          UMETA(DisplayName = "Key"),
    Key_1        UMETA(DisplayName = "Key_1"),
    Key_2        UMETA(DisplayName = "Key_2"),
    Key_3        UMETA(DisplayName = "Key_3"),
    Ball         UMETA(DisplayName = "Ball"),
    HealthPotion UMETA(DisplayName = "Health Potion"),
};