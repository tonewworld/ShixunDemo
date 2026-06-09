#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrabComponent.generated.h"

class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrabSuccess);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GAMEDEMO_API UGrabComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UGrabComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    void StartGrab();
    void TryGrab();
    void ReleaseGrab();
    bool IsGrabbing() const { return GrabbedActor != nullptr; }

    // ========== 新增：推/拉控制 ==========
    void StartPush();
    void StopPush();
    void StartPull();
    void StopPull();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab")
        float GrabRange = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab")
        bool bUsePhysics = true;

    UPROPERTY(BlueprintAssignable, Category = "Grab")
        FOnGrabSuccess OnGrabSuccess;

private:
    UPROPERTY()
        class UPrimitiveComponent* GrabbedComponent = nullptr;

    UPROPERTY()
        class AActor* GrabbedActor = nullptr;

    float GrabDistance = 0.0f;

    // ========== 新增：推/拉状态与参数 ==========
    bool bIsPushing = false;
    bool bIsPulling = false;

    UPROPERTY(EditAnywhere, Category = "PushPull")
        float PushPullSpeed = 200.0f;   // 距离变化速度（单位/秒）

    UPROPERTY(EditAnywhere, Category = "PushPull")
        float MinGrabDistance = 50.0f;

    UPROPERTY(EditAnywhere, Category = "PushPull")
        float MaxGrabDistance = 800.0f;

    class UCameraComponent* GetPlayerCamera() const;
};