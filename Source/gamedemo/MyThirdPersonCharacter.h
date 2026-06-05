#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyThirdPersonCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth);

UCLASS()
class GAMEDEMO_API AMyThirdPersonCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMyThirdPersonCharacter();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // 移动输入
    void MoveForward(float Value);
    void MoveRight(float Value);

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