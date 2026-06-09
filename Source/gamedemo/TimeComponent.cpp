#include "TimeComponent.h"
#include "MyThirdPersonCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

// Sets default values for this component's properties
UTimeComponent::UTimeComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    Owner = nullptr;
    IsInit = false;
    isTimeReversing = false;
    isOutData = false;
    RecordTimeLength = 0.0f;
    ReverseSpeed = 2;
}


// Called when the game starts
void UTimeComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!IsInit) {
        Owner = GetOwner();
        IsInit = true;
        AMyThirdPersonCharacter* myHero = Cast<AMyThirdPersonCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        myHero->TimeReverseDelegate.AddDynamic(this, &UTimeComponent::SetTimeReversing);
    }

}


// Called every frame
void UTimeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!IsInit) return;

    if (isTimeReversing)   // 回溯模式
    {
        if (isOutData) return;   // 没有更多数据可回溯

        // 根据 ReverseSpeed 决定本帧回退多少帧，至少为 1
        int32 Steps = FMath::Max(1, ReverseSpeed);

        for (int32 Step = 0; Step < Steps; ++Step)
        {
            auto TailNode = TimeFrames.GetTail();
            if (!TailNode)   // 链表为空
            {
                isOutData = true;
                RecordTimeLength = 0.0f;
                break;   // 没有更多帧，停止回溯
            }

            // 应用这一帧的状态
            const TimeInfo& Frame = TailNode->GetValue();
            Owner->SetActorLocation(Frame.Location);
            Owner->SetActorRotation(Frame.Rotation);

            // 获取 StaticMesh 组件并应用速度
            TArray<UStaticMeshComponent*> MeshComps;
            Owner->GetComponents<UStaticMeshComponent>(MeshComps);
            if (MeshComps.Num() > 0)
            {
                UStaticMeshComponent* usmc = MeshComps[0];
                usmc->SetPhysicsLinearVelocity(Frame.LinearVelocity);
                usmc->SetPhysicsAngularVelocityInDegrees(Frame.AngularVelocity);
            }

            // 移除已使用的帧
            RecordTimeLength -= Frame.DeltaTime;
            TimeFrames.RemoveNode(TailNode);

            if (TimeFrames.Num() == 0)
            {
                isOutData = true;
                RecordTimeLength = 0.0f;
                break;
            }
        }
    }
    else   // 正常记录模式
    {
        // 获取 StaticMesh 组件（使用现代 API）
        TArray<UStaticMeshComponent*> MeshComps;
        Owner->GetComponents<UStaticMeshComponent>(MeshComps);
        if (MeshComps.Num() == 0) return;

        UStaticMeshComponent* usmc = MeshComps[0];

        TimeInfo NewFrame(
            Owner->GetActorLocation(),
            Owner->GetActorRotation(),
            usmc->GetPhysicsLinearVelocity(),
            usmc->GetPhysicsAngularVelocityInDegrees(),
            DeltaTime
        );

        const float MaxRecordTime = 10.0f;
        if (RecordTimeLength <= MaxRecordTime)
        {
            TimeFrames.AddTail(NewFrame);
            RecordTimeLength += DeltaTime;
        }
        else
        {
            // 删除最旧的一帧
            auto HeadNode = TimeFrames.GetHead();
            if (HeadNode)
            {
                RecordTimeLength -= HeadNode->GetValue().DeltaTime;
                TimeFrames.RemoveNode(HeadNode);
            }
            TimeFrames.AddTail(NewFrame);
            RecordTimeLength += DeltaTime;
        }

        isOutData = false;   // 有数据可回溯
    }
}

void UTimeComponent::SetTimeReversing(bool InTimeReversing)
{
    isTimeReversing = InTimeReversing;

}