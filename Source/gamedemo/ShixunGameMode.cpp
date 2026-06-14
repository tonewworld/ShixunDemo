#include "ShixunGameMode.h"
#include "UObject/ConstructorHelpers.h"

AShixunGameMode::AShixunGameMode()
{
    // 默认 Pawn 使用 BP_ShixunCharacter（含网格体 + 时间回溯 + 磁力抓取）
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Assets/Blueprints/BP_ShixunCharacter.BP_ShixunCharacter_C"));
    if (PlayerPawnBPClass.Class != nullptr)
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }
}
