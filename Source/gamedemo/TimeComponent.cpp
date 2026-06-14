#include "TimeComponent.h"
#include "ShixunCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

UTimeComponent::UTimeComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    Owner = nullptr;
    IsInit = false;
    isTimeReversing = false;
    isOutData = false;
    RecordTimeLength = 0.0f;
    MaxRecordTime = 6.0f;
    ReverseSpeed = 2;
}

void UTimeComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!IsInit) {
        Owner = GetOwner();
        IsInit = true;
        AShixunCharacter* myHero = Cast<AShixunCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        if (myHero)
        {
            myHero->TimeReverseDelegate.AddDynamic(this, &UTimeComponent::SetTimeReversing);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("TimeComponent: Could not find ShixunCharacter!"));
        }
    }
}

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
            if (!TailNode)
            {
                isOutData = true;
                RecordTimeLength = 0.0f;
                break;
            }

            const TimeInfo& Frame = TailNode->GetValue();
            Owner->SetActorLocation(Frame.Location);
            Owner->SetActorRotation(Frame.Rotation);

            TArray<UStaticMeshComponent*> MeshComps;
            Owner->GetComponents<UStaticMeshComponent>(MeshComps);
            for (UStaticMeshComponent* usmc : MeshComps)
            {
                usmc->SetPhysicsLinearVelocity(Frame.LinearVelocity);
                usmc->SetPhysicsAngularVelocityInDegrees(Frame.AngularVelocity);
            }

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
        FVector LinearVel = FVector::ZeroVector;
        FVector AngularVel = FVector::ZeroVector;

        TArray<UStaticMeshComponent*> MeshComps;
        Owner->GetComponents<UStaticMeshComponent>(MeshComps);
        if (MeshComps.Num() > 0)
        {
            UStaticMeshComponent* usmc = MeshComps[0];
            LinearVel = usmc->GetPhysicsLinearVelocity();
            AngularVel = usmc->GetPhysicsAngularVelocityInDegrees();
        }

        TimeInfo NewFrame(
            Owner->GetActorLocation(),
            Owner->GetActorRotation(),
            LinearVel,
            AngularVel,
            DeltaTime
        );

        if (RecordTimeLength <= MaxRecordTime)
        {
            TimeFrames.AddTail(NewFrame);
            RecordTimeLength += DeltaTime;
        }
        else
        {
            auto HeadNode = TimeFrames.GetHead();
            if (HeadNode)
            {
                RecordTimeLength -= HeadNode->GetValue().DeltaTime;
                TimeFrames.RemoveNode(HeadNode);
            }
            TimeFrames.AddTail(NewFrame);
            RecordTimeLength += DeltaTime;
        }

        isOutData = false;
    }
}

void UTimeComponent::SetTimeReversing(bool InTimeReversing)
{
    isTimeReversing = InTimeReversing;
}
