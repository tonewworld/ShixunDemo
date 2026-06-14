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
	UTimeComponent();

protected:
	virtual void BeginPlay() override;

public:
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

	UFUNCTION(BlueprintPure, Category = "TimeReverse")
		bool IsReversing() const { return isTimeReversing; }
};
