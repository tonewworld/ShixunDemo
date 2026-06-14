#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

UCLASS()
class GAMEDEMO_API UMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // 目标关卡路径（可在蓝图中修改，当前用 TopDownExampleMap 代替 Map_1）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
    FName TargetLevel = FName("/Game/Assets/Maps/TopDownExampleMap");

protected:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "Menu")
    void StartGame();
};
