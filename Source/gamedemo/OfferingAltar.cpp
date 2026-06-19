#include "OfferingAltar.h"
#include "OfferingWidget.h"
#include "Components/BoxComponent.h"
#include "InventoryComponent.h"
#include "ShixunCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

AOfferingAltar::AOfferingAltar()
{
    PrimaryActorTick.bCanEverTick = true;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetBoxExtent(FVector(200, 200, 200));
    RootComponent = TriggerBox;

    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AOfferingAltar::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    if (GetWorld() && DestroyTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(DestroyTimer);
    }
}

void AOfferingAltar::BeginPlay()
{
    Super::BeginPlay();

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AOfferingAltar::OnTriggerBeginOverlap);
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AOfferingAltar::OnTriggerEndOverlap);

    if (StoneA) StoneAStart = StoneA->GetActorLocation();
    if (StoneB) StoneBStart = StoneB->GetActorLocation();
}

void AOfferingAltar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsMovingStones) return;

    MoveElapsed += DeltaTime;
    float Alpha = FMath::Clamp(MoveElapsed / StoneMoveDuration, 0.0f, 1.0f);
    float EasedAlpha = FMath::InterpEaseInOut(0.0f, 1.0f, Alpha, 2.0f);

    if (StoneA) StoneA->SetActorLocation(FMath::Lerp(StoneAStart, StoneATarget, EasedAlpha));
    if (StoneB) StoneB->SetActorLocation(FMath::Lerp(StoneBStart, StoneBTarget, EasedAlpha));

    if (Alpha >= 1.0f)
    {
        bIsMovingStones = false;
    }
}

void AOfferingAltar::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AShixunCharacter* Char = Cast<AShixunCharacter>(OtherActor);
    if (!Char) return;

    UInventoryComponent* Inventory = Char->InventoryComponent;
    if (!Inventory) return;

    APlayerController* PC = Cast<APlayerController>(Char->GetController());
    if (PC)
    {
        PC->SetIgnoreMoveInput(true);
        PC->bShowMouseCursor = true;
        FInputModeUIOnly InputMode;
        PC->SetInputMode(InputMode);
    }

    ActiveWidget = CreateWidget<UOfferingWidget>(GetWorld(), UOfferingWidget::StaticClass());
    if (!ActiveWidget) return;

    ActiveWidget->Init(Inventory);
    ActiveWidget->OnOfferComplete.AddDynamic(this, &AOfferingAltar::OnOfferComplete);
    ActiveWidget->AddToViewport();
}

void AOfferingAltar::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AShixunCharacter* Char = Cast<AShixunCharacter>(OtherActor);
    if (!Char) return;

    APlayerController* PC = Cast<APlayerController>(Char->GetController());
    if (PC)
    {
        PC->SetIgnoreMoveInput(false);
        PC->bShowMouseCursor = false;
        PC->SetInputMode(FInputModeGameOnly());
    }

    if (ActiveWidget)
    {
        ActiveWidget->RemoveFromViewport();
        ActiveWidget = nullptr;
    }
}

void AOfferingAltar::OnOfferComplete()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC)
    {
        PC->SetIgnoreMoveInput(false);
        PC->bShowMouseCursor = false;
        PC->SetInputMode(FInputModeGameOnly());
    }

    if (ActiveWidget)
    {
        ActiveWidget->RemoveFromViewport();
        ActiveWidget = nullptr;
    }

    // 禁用触发器，防止再次进入
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 播放音效
    if (OfferSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, OfferSound, GetActorLocation());
    }

    // 启动石门平移
    if (StoneA)
    {
        StoneAStart = StoneA->GetActorLocation();
        StoneATarget = StoneAStart + StoneAOffset;
    }
    if (StoneB)
    {
        StoneBStart = StoneB->GetActorLocation();
        StoneBTarget = StoneBStart + StoneBOffset;
    }
    bIsMovingStones = true;
    MoveElapsed = 0.0f;

    // 石门移动完毕后销毁祭坛
    GetWorldTimerManager().SetTimer(DestroyTimer, this, &AOfferingAltar::DestroySelf, StoneMoveDuration + 0.5f, false);
}

void AOfferingAltar::DestroySelf()
{
    Destroy();
}