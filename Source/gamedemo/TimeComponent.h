#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimeComponent.generated.h"

struct TimeInfo {
	FVector Location;
	FRotator Rotation;
	FVector LinearVelocity;
	FVector AngularVelocity;
	float DeltaTime;

	TimeInfo(FVector InLocation, FRotator InRotation, FVector InLinearVelocity, FVector InAngularVelocity, float InDeltaTime) :
		Location(InLocation), Rotation(InRotation), LinearVelocity(InLinearVelocity), AngularVelocity(InAngularVelocity), DeltaTime(InDeltaTime) {}
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class GAMEDEMO_API UTimeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTimeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	AActor* Owner;
	bool IsInit;
	bool isTimeReversing;
	bool isOutData;
	float RecordTimeLength;
	float MaxRecordTime;
	TDoubleLinkedList<TimeInfo> TimeFrames;

	UFUNCTION()
		void SetTimeReversing(bool InTimeReversing);

	UPROPERTY(EditAnywhere, Category = "TimeComponentInfo")
		int32 ReverseSpeed;

	// ====== 能量系统 ======
	UPROPERTY(EditAnywhere, Category = "TimeEnergy")
		float MaxEnergy;

	UPROPERTY(VisibleAnywhere, Category = "TimeEnergy")
		float CurrentEnergy;

	UPROPERTY(EditAnywhere, Category = "TimeEnergy")
		float EnergyDrainRate;

	UPROPERTY(EditAnywhere, Category = "TimeEnergy")
		float EnergyRestoreRate;

	// 蓝图函数：获取能量百分比（0~1），用于UI进度条绑定
	UFUNCTION(BlueprintPure, Category = "TimeEnergy")
		float GetEnergyPercentage() const;

	// 蓝图函数：是否正在回溯，用于UI颜色切换
	UFUNCTION(BlueprintPure, Category = "TimeEnergy")
		bool IsReversing() const { return isTimeReversing; }
};