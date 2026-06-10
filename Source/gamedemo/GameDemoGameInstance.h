#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameDemoGameInstance.generated.h"

class UGameManager;

UCLASS()
class GAMEDEMO_API UGameDemoGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UGameDemoGameInstance();

    // 初始化游戏管理器
    virtual void Init() override;
    virtual void Shutdown() override;

    /** 获取游戏管理器（全局唯一，跨关卡持久化） */
    UFUNCTION(BlueprintPure, Category = "GameManager")
        UGameManager* GetGameManager() const { return GameManager; }

private:
    UPROPERTY()
        UGameManager* GameManager;
};