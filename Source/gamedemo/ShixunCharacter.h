#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "ShixunCharacter.generated.h"

class UGrabComponent;
class UInventoryComponent;

UENUM(BlueprintType)
enum class EAbilityState : uint8
{
    Default         UMETA(DisplayName = "默认"),
    TimeRewind      UMETA(DisplayName = "时间回溯"),
    VisionScan      UMETA(DisplayName = "视野扫描"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTimeReverseDelegate, bool, IsTimeReversing);

UCLASS()
class GAMEDEMO_API AShixunCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AShixunCharacter();

protected:
    virtual void BeginPlay() override;
    bool bIsGrabKeyHeld = false;
    bool bWasReversingLastFrame = false;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // ===== 抓取 =====
    void OnGrabPressed();
    UFUNCTION()
        void OnGrabSuccess();
    void SetCrosshairColor(const FLinearColor& Color);
    void UpdateGrab();

    // ===== 移动 =====
    void MoveForward(float Value);
    void MoveRight(float Value);
    void OnJump();

    // ===== 时间回溯 =====
    void StartTimeReverse();
    void StopTimeReverse();
    FTimeReverseDelegate TimeReverseDelegate;

    // ===== 视野扫描 =====
    void StartVisionScan();
    void StopVisionScan();
    void RevealHiddenObjects(bool bReveal);

    // ===== 视角控制 =====
    void Turn(float Value);
    void LookUp(float Value);

    // ===== 推/拉 =====
    void OnPushPressed();
    void OnPushReleased();
    void OnPullPressed();
    void OnPullReleased();

    // ===== 组件 =====
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        class UTimeComponent* myTimeComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        class UGrabComponent* GrabComponent;

    // ===== 背包 =====
    void OnToggleInventory();
    void OnInteract();
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        class UInventoryComponent* InventoryComponent;

    // ===== 能力状态 =====
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability")
        EAbilityState CurrentState;

    // ===== 视野扫描参数 =====
    UPROPERTY(EditAnywhere, Category = "VisionScan")
        float VisionScanDuration;

    UPROPERTY(EditAnywhere, Category = "VisionScan")
        float VisionScanCooldown;

    UPROPERTY(VisibleAnywhere, Category = "VisionScan")
        float VisionScanCooldownRemaining;

    UPROPERTY(VisibleAnywhere, Category = "VisionScan")
        float VisionScanTimer;

    // ===== 时间回溯 CD =====
    UPROPERTY(EditAnywhere, Category = "Cooldown")
        float TimeRewindCooldown;

    UPROPERTY(VisibleAnywhere, Category = "Cooldown")
        float TimeRewindCooldownRemaining;

    UFUNCTION(BlueprintPure, Category = "Cooldown")
        float GetTimeRewindCooldownPercentage() const;

    // ===== UI =====
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
        TSubclassOf<UUserWidget> CrosshairWidgetClass;

    UPROPERTY()
        UUserWidget* CrosshairWidgetInstance;

    // ===== 血量 =====
    UFUNCTION(BlueprintCallable, Category = "Health")
        void ApplyDamage(float DamageAmount);

    UFUNCTION(BlueprintCallable, Category = "Health")
        void Heal(float HealAmount);

    UFUNCTION(BlueprintPure, Category = "Health")
        float GetCurrentHealth() const { return Health; }

    UFUNCTION(BlueprintPure, Category = "Health")
        float GetMaxHealth() const { return MaxHealth; }

    UPROPERTY(BlueprintAssignable, Category = "Health")
        FOnHealthChanged OnHealthChanged;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (AllowPrivateAccess = "true"))
        float MaxHealth;

    /** 记录上次安全的重生位置 */
    UPROPERTY(VisibleAnywhere, Category = "Health")
        FVector LastSpawnLocation;

    UPROPERTY(VisibleAnywhere, Category = "Health")
        FRotator LastSpawnRotation;

    /** 死亡时重置血量并传送到重生点 */
    void RespawnAtLastSpawnPoint();

    /** 重生完成后重置标记 */
    void FinishRespawn();

    /** 是否正在重生中（无敌状态） */
    bool IsRespawning() const { return bIsRespawning; }

    bool IsTimeReversing() const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
        class UCameraComponent* FollowCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
        float Health;

    /** 是否正在重生中（无敌状态） */
    bool bIsRespawning = false;
};