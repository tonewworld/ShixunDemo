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

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void StartGrab();
    void ReleaseGrab();
    bool IsGrabbing() const { return GrabbedActor != nullptr; }

    // ========== ��������/������ ==========
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

    // ========== ��������/��״̬����� ==========
    bool bIsPushing = false;
    bool bIsPulling = false;

    UPROPERTY(EditAnywhere, Category = "PushPull")
        float PushPullSpeed = 200.0f;   // ����仯�ٶȣ���λ/�룩

    UPROPERTY(EditAnywhere, Category = "PushPull")
        float MinGrabDistance = 50.0f;

    UPROPERTY(EditAnywhere, Category = "PushPull")
        float MaxGrabDistance = 800.0f;

    class UCameraComponent* GetPlayerCamera() const;
};