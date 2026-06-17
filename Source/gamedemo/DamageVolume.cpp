#include "DamageVolume.h"
#include "ShixunCharacter.h"
#include "Components/PrimitiveComponent.h"

UDamageVolume::UDamageVolume()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UDamageVolume::BeginPlay()
{
    Super::BeginPlay();

    // 自动找到 Owner 上启用了碰撞的 PrimitiveComponent 绑定事件
    AActor* Owner = GetOwner();
    if (!Owner) return;

    // 尝试获取根组件作为碰撞触发器
    UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Owner->GetRootComponent());
    if (PrimComp)
    {
        PrimComp->OnComponentBeginOverlap.AddDynamic(this, &UDamageVolume::OnOverlapBegin);
        PrimComp->OnComponentEndOverlap.AddDynamic(this, &UDamageVolume::OnOverlapEnd);

        // 确保能产生重叠事件
        PrimComp->SetGenerateOverlapEvents(true);
        PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        PrimComp->SetCollisionObjectType(ECC_WorldDynamic);
        PrimComp->SetCollisionResponseToAllChannels(ECR_Ignore);
        PrimComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UDamageVolume: Owner 的根组件不是 UPrimitiveComponent，无法绑定重叠事件"));
    }
}

void UDamageVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (DamageTimerHandle.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(DamageTimerHandle);
    }
}

void UDamageVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    AShixunCharacter* Char = Cast<AShixunCharacter>(OtherActor);
    if (!Char) return;

    // 单次伤害模式：碰一下就扣血，不持续
    if (bOneTimeDamage)
    {
        Char->ApplyDamage(DamageAmount);
        return;
    }

    // 持续伤害模式：定时器反复扣血
    PlayerInVolume = Char;

    // 立即造成一次伤害
    ApplyDamageToPlayer();

    // 启动定时器持续伤害
    float Interval = HitsPerSecond > 0.0f ? (1.0f / HitsPerSecond) : 1.0f;
    Interval = FMath::Max(Interval, 0.1f);
    GetWorld()->GetTimerManager().SetTimer(DamageTimerHandle, this, &UDamageVolume::ApplyDamageToPlayer, Interval, true);
}

void UDamageVolume::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AShixunCharacter* Char = Cast<AShixunCharacter>(OtherActor);
    if (!Char) return;

    if (PlayerInVolume.Get() == Char)
    {
        PlayerInVolume = nullptr;

        if (DamageTimerHandle.IsValid())
        {
            GetWorld()->GetTimerManager().ClearTimer(DamageTimerHandle);
        }
    }
}

void UDamageVolume::ApplyDamageToPlayer()
{
    AShixunCharacter* Char = PlayerInVolume.Get();
    if (!Char) return;

    Char->ApplyDamage(DamageAmount);
}