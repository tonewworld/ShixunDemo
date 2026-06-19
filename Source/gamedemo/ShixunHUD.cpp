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
    float MaxHealth = Char->GetMaxHealth();
    if (MaxHealth <= 0.0f) return 1.0f;
    return Char->GetCurrentHealth() / MaxHealth;
}

float UShixunHUD::GetMaxHealth() const
{
    AShixunCharacter* Char = GetPlayerCharacter();
    if (!Char) return 100.0f;
    return Char->GetMaxHealth();
}

float UShixunHUD::GetCurrentHealth() const
{
    AShixunCharacter* Char = GetPlayerCharacter();
    if (!Char) return 100.0f;
    return Char->GetCurrentHealth();
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