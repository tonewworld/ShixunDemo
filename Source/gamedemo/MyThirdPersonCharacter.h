#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "MyThirdPersonCharacter.generated.h"

class UGrabComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTimeReverseDelegate, bool, IsTimeReversing);

UCLASS()
class GAMEDEMO_API AMyThirdPersonCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMyThirdPersonCharacter();

protected:
    virtual void BeginPlay() override;
    bool bIsGrabKeyHeld = false;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // 抓取 UI 控制
    void OnGrabPressed();
    void OnGrabReleased();

    // 抓取成功时的回调（由 GrabComponent 的委托触发）
    UFUNCTION()
        void OnGrabSuccess();

    // 设置准心颜色（如果准心存在）
    void SetCrosshairColor(const FLinearColor& Color);

    // 每帧执行抓取检测（在 Tick 中调用）
    void UpdateGrab();

    // 移动输入
    void MoveForward(float Value);
    void MoveRight(float Value);

    // 时间回溯（物体）
    void StartTimeReverse();
    void StopTimeReverse();
    FTimeReverseDelegate TimeReverseDelegate;

    // 时间回溯（人）
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TimeComponentInfo")
        class UTimeComponent* myTimeComponent;

    // 隔空抓取
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grab")
        class UGrabComponent* GrabComponent;

    // 准心 UI 的蓝图类（在编辑器中指定）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
        TSubclassOf<UUserWidget> CrosshairWidgetClass;

    // 准心实例
    UPROPERTY()
        UUserWidget* CrosshairWidgetInstance;

    // ========== 视角控制（鼠标） ==========
    void Turn(float Value);
    void LookUp(float Value);

    // 伤害与血量系统
    UFUNCTION(BlueprintCallable, Category = "Health")
        void ApplyDamage(float DamageAmount);

    UFUNCTION(BlueprintCallable, Category = "Health")
        void TestDamage();

    UFUNCTION(BlueprintPure, Category = "Health")
        float GetCurrentHealth() const { return Health; }

    UFUNCTION(BlueprintPure, Category = "Health")
        float GetMaxHealth() const { return MaxHealth; }

    UPROPERTY(BlueprintAssignable, Category = "Health")
        FOnHealthChanged OnHealthChanged;

    // ========== 能量UI转发函数 ==========
    UFUNCTION(BlueprintPure, Category = "TimeEnergy")
        float GetTimeEnergyPercentage() const;

    UFUNCTION(BlueprintPure, Category = "TimeEnergy")
        bool IsTimeReversing() const;

    // ========== 新增：推/拉回调 ==========
    void OnPushPressed();
    void OnPushReleased();
    void OnPullPressed();
    void OnPullReleased();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
        class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
        class UCameraComponent* FollowCamera;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
        float Health;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
        float MaxHealth;
};