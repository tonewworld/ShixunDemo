#include "GrabComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "MagneticInteractable.h"

UGrabComponent::UGrabComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UGrabComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!GrabbedComponent || !GrabbedActor) return;

    UCameraComponent* Camera = GetPlayerCamera();
    if (!Camera) return;

    // 计算目标位置（相机前方固定距离）
    FVector TargetLocation = Camera->GetComponentLocation() + Camera->GetForwardVector() * GrabDistance;

    if (bUsePhysics)
    {
        FVector CurrentLoc = GrabbedComponent->GetComponentLocation();
        FVector Delta = TargetLocation - CurrentLoc;
        FVector Velocity = Delta / DeltaTime;
        Velocity = Velocity.GetClampedToMaxSize(800.0f);
        GrabbedComponent->SetPhysicsLinearVelocity(Velocity, false);
    }
    else
    {
        GrabbedComponent->SetWorldLocation(TargetLocation, false, nullptr, ETeleportType::TeleportPhysics);
    }

    // 水平旋转
    float YawVelocity = 0.0f;
    if (bIsRotatingLeft)
        YawVelocity = RotationSpeed;
    else if (bIsRotatingRight)
        YawVelocity = -RotationSpeed;

    if (YawVelocity != 0.0f)
    {
        if (bUsePhysics)
        {
            GrabbedComponent->SetPhysicsAngularVelocityInDegrees(FVector(0.0f, 0.0f, YawVelocity), false, NAME_None);
        }
        else
        {
            FRotator CurrentRot = GrabbedActor->GetActorRotation();
            CurrentRot.Yaw += YawVelocity * DeltaTime;
            GrabbedActor->SetActorRotation(CurrentRot);
        }
    }
}

void UGrabComponent::StartGrab()
{
    if (GrabbedComponent) return;

    ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
    if (!OwnerChar) return;

    APlayerController* PC = Cast<APlayerController>(OwnerChar->GetController());
    if (!PC) return;

    int32 ViewportX, ViewportY;
    PC->GetViewportSize(ViewportX, ViewportY);
    FVector2D CrosshairPosition(ViewportX * 0.5f, ViewportY * 0.5f);

    FHitResult Hit;
    bool bHit = PC->GetHitResultAtScreenPosition(CrosshairPosition, ECC_Visibility, true, Hit);

    if (bHit && Hit.Distance <= GrabRange)
    {
        UPrimitiveComponent* PrimComp = Hit.GetComponent();
        AActor* HitActor = Hit.GetActor();

        // 检查是否实现了 IMagneticInteractable 接口
        if (!HitActor || !HitActor->GetClass()->ImplementsInterface(UMagneticInteractable::StaticClass()))
        {
            return;
        }

        // 检查质量限制 < 500kg
        if (PrimComp && PrimComp->IsSimulatingPhysics())
        {
            float Mass = PrimComp->GetMass();
            if (Mass >= 500.0f)
            {
                return;
            }

            // 通知接口
            IMagneticInteractable::Execute_OnGrabbed(HitActor, GetOwner());

            GrabbedComponent = PrimComp;
            GrabbedActor = HitActor;
            OnGrabSuccess.Broadcast();

            UCameraComponent* Camera = GetPlayerCamera();
            if (Camera)
            {
                FVector CameraLoc = Camera->GetComponentLocation();
                FVector ObjectLoc = GrabbedComponent->GetComponentLocation();
                GrabDistance = FVector::Dist(ObjectLoc, CameraLoc);
            }

            GrabbedComponent->SetEnableGravity(false);
            GrabbedComponent->SetLinearDamping(5.0f);
            GrabbedComponent->SetAngularDamping(5.0f);
        }
    }
}

void UGrabComponent::ReleaseGrab()
{
    if (GrabbedActor)
    {
        IMagneticInteractable::Execute_OnMagneticRelease(GrabbedActor);
    }

    if (GrabbedComponent)
    {
        GrabbedComponent->SetEnableGravity(true);
        GrabbedComponent->SetLinearDamping(0.0f);
        GrabbedComponent->SetAngularDamping(0.0f);
    }
    GrabbedActor = nullptr;
    GrabbedComponent = nullptr;

    bIsRotatingLeft = false;
    bIsRotatingRight = false;
}

UCameraComponent* UGrabComponent::GetPlayerCamera() const
{
    ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
    if (OwnerChar)
    {
        return OwnerChar->FindComponentByClass<UCameraComponent>();
    }
    return nullptr;
}

void UGrabComponent::StartRotateLeft()
{
    bIsRotatingLeft = true;
    bIsRotatingRight = false;
}

void UGrabComponent::StopRotateLeft()
{
    bIsRotatingLeft = false;
}

void UGrabComponent::StartRotateRight()
{
    bIsRotatingRight = true;
    bIsRotatingLeft = false;
}

void UGrabComponent::StopRotateRight()
{
    bIsRotatingRight = false;
}