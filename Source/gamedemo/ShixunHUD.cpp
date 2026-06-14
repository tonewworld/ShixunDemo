#include "ShixunHUD.h"
#include "ShixunCharacter.h"
#include "GameFramework/PlayerController.h"

AShixunCharacter* UShixunHUD::GetPlayerCharacter() const
{
    APlayerController* PC = GetOwningPlayer();
    if (!PC) return nullptr;
    return Cast<AShixunCharacter>(PC->GetPawn());
}

// ===== 血量 =====
float UShixunHUD::GetHealthPercent() const
{
    AShixunCharacter* Char = GetPlayerCharacter();
    if (!Char) return 1.0f;
    float MaxHP = Char->GetMaxHealth();
    if (MaxHP <= 0.0f) return 1.0f;
    return Char->GetCurrentHealth() / MaxHP;
}

float UShixunHUD::GetMaxHealth() const
{
    AShixunCharacter* Char = GetPlayerCharacter();
    return Char ? Char->GetMaxHealth() : 100.0f;
}

float UShixunHUD::GetCurrentHealth() const
{
    AShixunCharacter* Char = GetPlayerCharacter();
    return Char ? Char->GetCurrentHealth() : 0.0f;
}

// ===== 时间回溯 CD =====
float UShixunHUD::GetTimeRewindCooldownPercent() const
{
    AShixunCharacter* Char = GetPlayerCharacter();
    if (!Char) return 0.0f;
    return Char->GetTimeRewindCooldownPercentage();
}

bool UShixunHUD::IsTimeRewindReady() const
{
    AShixunCharacter* Char = GetPlayerCharacter();
    if (!Char) return false;
    return Char->GetTimeRewindCooldownPercentage() <= 0.0f;
}

// ===== 视野扫描 CD =====
float UShixunHUD::GetVisionScanCooldownPercent() const
{
    AShixunCharacter* Char = GetPlayerCharacter();
    if (!Char) return 0.0f;
    return FMath::GetMappedRangeValueClamped(
        FVector2D(0.0f, 6.0f), FVector2D(0.0f, 1.0f),
        Char->VisionScanCooldownRemaining);
}

bool UShixunHUD::IsVisionScanReady() const
{
    AShixunCharacter* Char = GetPlayerCharacter();
    if (!Char) return false;
    return Char->VisionScanCooldownRemaining <= 0.0f;
}

// ===== 磁力抓取 =====
bool UShixunHUD::IsGrabReady() const
{
    return true;
}

// ===== 技能图标颜色 =====
FLinearColor UShixunHUD::GetTimeRewindIconColor() const
{
    return IsTimeRewindReady() ? FLinearColor::White : FLinearColor(0.15f, 0.15f, 0.15f, 1.0f);
}

FLinearColor UShixunHUD::GetGrabIconColor() const
{
    return FLinearColor::White;
}

FLinearColor UShixunHUD::GetVisionScanIconColor() const
{
    return IsVisionScanReady() ? FLinearColor::White : FLinearColor(0.15f, 0.15f, 0.15f, 1.0f);
}
