#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShixunHUD.generated.h"

class AShixunCharacter;

UCLASS()
class GAMEDEMO_API UShixunHUD : public UUserWidget
{
    GENERATED_BODY()

public:
    // ===== 血量 =====
    UFUNCTION(BlueprintPure, Category = "HUD")
    float GetHealthPercent() const;

    UFUNCTION(BlueprintPure, Category = "HUD")
    float GetMaxHealth() const;

    UFUNCTION(BlueprintPure, Category = "HUD")
    float GetCurrentHealth() const;

    // ===== 时间回溯 CD =====
    UFUNCTION(BlueprintPure, Category = "HUD")
    float GetTimeRewindCooldownPercent() const;

    UFUNCTION(BlueprintPure, Category = "HUD")
    bool IsTimeRewindReady() const;

    // ===== 磁力抓取（0 CD，始终可用） =====
    UFUNCTION(BlueprintPure, Category = "HUD")
    bool IsGrabReady() const;

    // ===== 技能图标颜色（白=可用，灰=CD中） =====
    UFUNCTION(BlueprintPure, Category = "HUD")
    FLinearColor GetTimeRewindIconColor() const;

    UFUNCTION(BlueprintPure, Category = "HUD")
    FLinearColor GetGrabIconColor() const;

private:
    AShixunCharacter* GetPlayerCharacter() const;
};
