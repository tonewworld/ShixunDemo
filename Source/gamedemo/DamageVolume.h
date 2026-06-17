#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageVolume.generated.h"

class AShixunCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GAMEDEMO_API UDamageVolume : public UActorComponent
{
    GENERATED_BODY()

public:
    UDamageVolume();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /** 每次触发造成的伤害量 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageVolume", meta = (ClampMin = "0.0"))
    float DamageAmount = 10.0f;

    /** true = 单次伤害（碰一下扣一次血），false = 持续伤害（站在上面持续扣血） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageVolume")
    bool bOneTimeDamage = true;

    /** 持续伤害模式下，每秒扣血的次数（仅 bOneTimeDamage=false 时生效） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageVolume", meta = (ClampMin = "0.1", EditCondition = "!bOneTimeDamage"))
    float HitsPerSecond = 1.0f;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    /** 对当前在区域内的玩家造成一次伤害 */
    UFUNCTION()
    void ApplyDamageToPlayer();

private:
    /** 当前在区域内的玩家 */
    TWeakObjectPtr<AShixunCharacter> PlayerInVolume;

    /** 持续伤害定时器句柄 */
    FTimerHandle DamageTimerHandle;
};