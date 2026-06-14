#include "TimeComponent.h"
#include "ShixunCharacter.h"
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
		MaxRecordTime = 6.0f;
		ReverseSpeed = 2;
		MaxEnergy = 300.0f;
		CurrentEnergy = MaxEnergy;
		EnergyDrainRate = 100.0f;
		EnergyRestoreRate = 30.0f;
}


// Called when the game starts
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


// Called every frame
void UTimeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!IsInit) return;

		if (isTimeReversing)   // 回溯模式
		{
			if (isOutData) return;   // 没有更多数据可回溯

			// 能量消耗
			CurrentEnergy -= EnergyDrainRate * DeltaTime;
			if (CurrentEnergy <= 0.0f)
			{
				CurrentEnergy = 0.0f;
				isTimeReversing = false;
				return;
			}

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

				// 遍历所有 StaticMesh 组件，修复硬编码 [0] 问题
				TArray<UStaticMeshComponent*> MeshComps;
				Owner->GetComponents<UStaticMeshComponent>(MeshComps);
				for (UStaticMeshComponent* usmc : MeshComps)
				{
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
			// 能量恢复
			CurrentEnergy = FMath::Min(CurrentEnergy + EnergyRestoreRate * DeltaTime, MaxEnergy);

			// 获取所有 StaticMesh 组件（修复硬编码 [0] 问题）
			TArray<UStaticMeshComponent*> MeshComps;
			Owner->GetComponents<UStaticMeshComponent>(MeshComps);
			if (MeshComps.Num() == 0) return;

			// 取第一个组件的速度作为参考（所有组件速度一致）
			UStaticMeshComponent* usmc = MeshComps[0];

			TimeInfo NewFrame(
				Owner->GetActorLocation(),
				Owner->GetActorRotation(),
				usmc->GetPhysicsLinearVelocity(),
				usmc->GetPhysicsAngularVelocityInDegrees(),
				DeltaTime
			);

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

float UTimeComponent::GetEnergyPercentage() const
{
    if (MaxEnergy <= 0.0f) return 0.0f;
    return CurrentEnergy / MaxEnergy;
}

void UTimeComponent::SetTimeReversing(bool InTimeReversing)
{
    isTimeReversing = InTimeReversing;

}
