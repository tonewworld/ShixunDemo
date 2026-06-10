#include "GameManager.h"
#include "GameDemoGameInstance.h"
#include "Engine/World.h"

UGameManager::UGameManager()
{
    CurrentState = EGameState::None;
}

void UGameManager::OnInit()
{
    CurrentState = EGameState::Init;
    OnGameStateChanged.Broadcast(EGameState::None, EGameState::Init);
}

void UGameManager::SetGameState(EGameState NewState)
{
    if (CurrentState == NewState) return;

    EGameState OldState = CurrentState;
    CurrentState = NewState;
    OnGameStateChanged.Broadcast(OldState, NewState);
}

UGameManager* UGameManager::GetGameManager(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;

    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return nullptr;

    UGameDemoGameInstance* GI = Cast<UGameDemoGameInstance>(World->GetGameInstance());
    if (!GI) return nullptr;

    return GI->GetGameManager();
}