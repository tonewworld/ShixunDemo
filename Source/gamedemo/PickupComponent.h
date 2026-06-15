#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemID.h"
#include "PickupComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GAMEDEMO_API UPickupComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPickupComponent() = default;

	/** 拾取后添加到背包的物品 ID */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	EItemID ItemID = EItemID::Key;
};
