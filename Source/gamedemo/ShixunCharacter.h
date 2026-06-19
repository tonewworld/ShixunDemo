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
    void OnGrabReleased();
    UFUNCTION()
        void OnGrabSuccess();
    void SetCrosshairColor(const FLinearColor& Color);
    void UpdateGrab();

    // ===== 移动 =====
    void MoveForward(float Value);
    void MoveRight(float Value);
    void OnJump();
    void OnSprintPressed();
    void OnSprintReleased();
    void OnCrouchPressed();
    void OnCrouchReleased();

    UPROPERTY(EditAnywhere, Category = "Movement")
        float WalkSpeed = 600.0f;

    UPROPERTY(EditAnywhere, Category = "Movement")
        float SprintSpeed = 900.0f;

    UPROPERTY(EditAnywhere, Category = "Movement")
        float CrouchSpeed = 300.0f;

    UPROPERTY(EditAnywhere, Category = "Movement")
        float JumpZVelocity = 600.0f;

    UPROPERTY(EditAnywhere, Category = "Movement")
        float AirControl = 0.2f;

    UPROPERTY(EditAnywhere, Category = "Movement")
        float GravityScale = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Movement")
        float RotationRateYaw = 540.0f;

    UPROPERTY(EditAnywhere, Category = "Movement")
        float MaxAcceleration = 2048.0f;

    UPROPERTY(EditAnywhere, Category = "Movement")
        float BrakingDeceleration = 2048.0f;

    UPROPERTY(EditAnywhere, Category = "Movement")
        float GroundFriction = 8.0f;

    UPROPERTY(EditAnywhere, Category = "Movement")
        float CrouchCameraZ = 32.0f;

    UPROPERTY(EditAnywhere, Category = "Movement")
        float CrouchInterpSpeed = 8.0f;

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

    // ===== 旋转抓取物 =====
    void OnRotateLeftPressed();
    void OnRotateLeftReleased();
    void OnRotateRightPressed();
    void OnRotateRightReleased();

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

    /** 是否正在冲刺 */
    bool bIsSprinting = false;

    /** 是否正在蹲下 */
    bool bIsCrouching = false;

    /** 站立时相机高度（运行时从 FollowCamera 初始 Z 获取） */
    float StandCameraZ = 64.0f;

    /** 每帧插值相机高度 */
    void UpdateCrouch(float DeltaTime);

    /** 应用 3C 参数到 CharacterMovementComponent */
    void ApplyMovementParams();
};