#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameManager.generated.h"

// ========== 游戏状态枚举 ==========
UENUM(BlueprintType)
enum class EGameState : uint8
{
    None        UMETA(DisplayName = "无"),
    Init        UMETA(DisplayName = "初始化"),
    Menu        UMETA(DisplayName = "主菜单"),
    Playing     UMETA(DisplayName = "游戏进行中"),
    Paused      UMETA(DisplayName = "暂停"),
    GameOver    UMETA(DisplayName = "游戏结束"),
};

// ========== 游戏状态变化委托 ==========
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameStateChanged, EGameState, OldState, EGameState, NewState);

// ========== 游戏管理器 ==========
UCLASS(Blueprintable, BlueprintType)
class GAMEDEMO_API UGameManager : public UObject
{
    GENERATED_BODY()

public:
    UGameManager();

    /** 游戏管理器初始化（GameInstance创建时调用） */
    UFUNCTION(BlueprintCallable, Category = "GameManager")
        void OnInit();

    /** 切换到指定游戏状态 */
    UFUNCTION(BlueprintCallable, Category = "GameManager")
        void SetGameState(EGameState NewState);

    /** 获取当前游戏状态 */
    UFUNCTION(BlueprintPure, Category = "GameManager")
        FString GetCurrentGameState() const { return UEnum::GetValueAsString(CurrentState); }

    /** 获取游戏管理器实例 */
    UFUNCTION(BlueprintPure, Category = "GameManager", meta = (WorldContext = "WorldContextObject"))
        static UGameManager* GetGameManager(const UObject* WorldContextObject);

    // ===== 委托 =====
    /** 游戏状态变化时广播：旧状态，新状态 */
    UPROPERTY(BlueprintAssignable, Category = "GameManager")
        FOnGameStateChanged OnGameStateChanged;

private:
    EGameState CurrentState;
};