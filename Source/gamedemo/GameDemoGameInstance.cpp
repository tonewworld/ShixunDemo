#include "GameDemoGameInstance.h"
#include "GameManager.h"

UGameDemoGameInstance::UGameDemoGameInstance()
{
    GameManager = nullptr;
}

void UGameDemoGameInstance::Init()
{
    Super::Init();

    // 创建游戏管理器（跨关卡持久化）
    GameManager = NewObject<UGameManager>(this, UGameManager::StaticClass(), TEXT("GameManager"));
    if (GameManager)
    {
        GameManager->OnInit();
    }
}

void UGameDemoGameInstance::Shutdown()
{
    // 清理
    GameManager = nullptr;

    Super::Shutdown();
}
