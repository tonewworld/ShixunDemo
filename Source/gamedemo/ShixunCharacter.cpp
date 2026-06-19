#include "ShixunCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "GrabComponent.h"
#include "TimeComponent.h"
#include "InventoryComponent.h"
#include "PickupComponent.h"
#include "Components/Image.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

AShixunCharacter::AShixunCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // 第一人称相机（在胶囊体中心，胶囊体碰撞防止穿墙）
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(RootComponent);
    FollowCamera->SetRelativeLocation(FVector(0.0f, 0.0f, StandCameraZ));
    FollowCamera->bUsePawnControlRotation = true;

    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->bOrientRotationToMovement = false;
        MoveComp->bUseControllerDesiredRotation = true;
    }

    bUseControllerRotationYaw = true;
    bUseControllerRotationPitch = true;
    bUseControllerRotationRoll = false;

    GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

    MaxHealth = 100.0f;
    Health = MaxHealth;

    myTimeComponent = CreateDefaultSubobject<UTimeComponent>(TEXT("myTimeComponent"));
    GrabComponent = CreateDefaultSubobject<UGrabComponent>(TEXT("GrabComponent"));
    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

    TimeRewindCooldown = 5.0f;
    TimeRewindCooldownRemaining = 0.0f;

    CurrentState = EAbilityState::Default;
}

void AShixunCharacter::BeginPlay()
{
    Super::BeginPlay();

    ApplyMovementParams();

    // 记录编辑器设置的初始相机高度
    StandCameraZ = FollowCamera->GetRelativeLocation().Z;

    OnHealthChanged.Broadcast(Health, MaxHealth);

    // 记录初始位置作为默认重生点
    LastSpawnLocation = GetActorLocation();
    LastSpawnRotation = GetActorRotation();

    if (CrosshairWidgetClass)
    {
        CrosshairWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetClass);
        SetCrosshairColor(FLinearColor::White);
    }
    if (GrabComponent)
    {
        GrabComponent->OnGrabSuccess.AddDynamic(this, &AShixunCharacter::OnGrabSuccess);
    }
}

void AShixunCharacter::ApplyMovementParams()
{
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (!MoveComp) return;

    MoveComp->MaxWalkSpeed = WalkSpeed;
    MoveComp->JumpZVelocity = JumpZVelocity;
    MoveComp->AirControl = AirControl;
    MoveComp->GravityScale = GravityScale;
    MoveComp->RotationRate = FRotator(0.0f, RotationRateYaw, 0.0f);
    MoveComp->MaxAcceleration = MaxAcceleration;
    MoveComp->BrakingDecelerationWalking = BrakingDeceleration;
    MoveComp->GroundFriction = GroundFriction;
}

void AShixunCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateGrab();
    UpdateCrouch(DeltaTime);

    // ===== 状态机 =====
    switch (CurrentState)
    {
    case EAbilityState::TimeRewind:
        if (myTimeComponent)
        {
            bool bIsNowReversing = myTimeComponent->isTimeReversing;
            if (bWasReversingLastFrame && !bIsNowReversing)
            {
                TimeRewindCooldownRemaining = TimeRewindCooldown;
                CurrentState = EAbilityState::Default;
            }
            bWasReversingLastFrame = bIsNowReversing;
        }

        if (TimeRewindCooldownRemaining > 0)
            TimeRewindCooldownRemaining -= DeltaTime;

        if (myTimeComponent && myTimeComponent->isTimeReversing && Controller)
        {
            auto TailNode = myTimeComponent->TimeFrames.GetTail();
            if (TailNode)
            {
                Controller->SetControlRotation(TailNode->GetValue().Rotation);
            }
        }
        break;

    default:
        if (TimeRewindCooldownRemaining > 0)
            TimeRewindCooldownRemaining -= DeltaTime;
        break;
    }
}

void AShixunCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AShixunCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AShixunCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &AShixunCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AShixunCharacter::LookUp);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShixunCharacter::OnJump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
    PlayerInputComponent->BindAction("TimeReverse", IE_Pressed, this, &AShixunCharacter::StartTimeReverse);
    PlayerInputComponent->BindAction("TimeReverse", IE_Released, this, &AShixunCharacter::StopTimeReverse);
    PlayerInputComponent->BindAction("Grab", IE_Pressed, this, &AShixunCharacter::OnGrabPressed);
    PlayerInputComponent->BindAction("Grab", IE_Released, this, &AShixunCharacter::OnGrabReleased);

    PlayerInputComponent->BindAction("Push", IE_Pressed, this, &AShixunCharacter::OnRotateLeftPressed);
    PlayerInputComponent->BindAction("Push", IE_Released, this, &AShixunCharacter::OnRotateLeftReleased);
    PlayerInputComponent->BindAction("Pull", IE_Pressed, this, &AShixunCharacter::OnRotateRightPressed);
    PlayerInputComponent->BindAction("Pull", IE_Released, this, &AShixunCharacter::OnRotateRightReleased);

    // 冲刺 Shift 键
    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AShixunCharacter::OnSprintPressed);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AShixunCharacter::OnSprintReleased);

    // 蹲下 Ctrl 键
    PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShixunCharacter::OnCrouchPressed);
    PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AShixunCharacter::OnCrouchReleased);

    // 背包 Tab 键
    PlayerInputComponent->BindKey(EKeys::Tab, IE_Pressed, this, &AShixunCharacter::OnToggleInventory);
    // 交互 E 键
    PlayerInputComponent->BindKey(EKeys::E, IE_Pressed, this, &AShixunCharacter::OnInteract);
}

void AShixunCharacter::MoveForward(float Value)
{
    if (Controller && Value != 0.0f)
    {
        const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AShixunCharacter::MoveRight(float Value)
{
    if (Controller && Value != 0.0f)
    {
        const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}

void AShixunCharacter::OnJump()
{
    if (InventoryComponent && InventoryComponent->bInventoryOpen) return;
    Jump();
}

void AShixunCharacter::OnSprintPressed()
{
    if (InventoryComponent && InventoryComponent->bInventoryOpen) return;
    bIsSprinting = true;
    if (!bIsCrouching)
    {
        if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
            MoveComp->MaxWalkSpeed = SprintSpeed;
    }
}

void AShixunCharacter::OnSprintReleased()
{
    bIsSprinting = false;
    if (!bIsCrouching)
    {
        if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
            MoveComp->MaxWalkSpeed = WalkSpeed;
    }
}

void AShixunCharacter::OnCrouchPressed()
{
    if (InventoryComponent && InventoryComponent->bInventoryOpen) return;
    bIsCrouching = true;
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
        MoveComp->MaxWalkSpeed = CrouchSpeed;
}

void AShixunCharacter::OnCrouchReleased()
{
    bIsCrouching = false;
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
        MoveComp->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
}

void AShixunCharacter::UpdateCrouch(float DeltaTime)
{
    float TargetZ = bIsCrouching ? CrouchCameraZ : StandCameraZ;
    FVector CamLoc = FollowCamera->GetRelativeLocation();
    CamLoc.Z = FMath::FInterpTo(CamLoc.Z, TargetZ, DeltaTime, CrouchInterpSpeed);
    FollowCamera->SetRelativeLocation(CamLoc);
}

void AShixunCharacter::Turn(float Value)
{
    if (myTimeComponent && myTimeComponent->isTimeReversing) return;
    if (InventoryComponent && InventoryComponent->bInventoryOpen) return;
    AddControllerYawInput(Value);
}

void AShixunCharacter::LookUp(float Value)
{
    if (myTimeComponent && myTimeComponent->isTimeReversing) return;
    if (InventoryComponent && InventoryComponent->bInventoryOpen) return;
    AddControllerPitchInput(Value);
}

// ===== 血量系统 =====
void AShixunCharacter::ApplyDamage(float DamageAmount)
{
    if (DamageAmount <= 0.0f) return;
    if (bIsRespawning) return;

    Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(Health, MaxHealth);

    UE_LOG(LogTemp, Log, TEXT("受到伤害: %.1f, 当前血量: %.1f/%.1f"), DamageAmount, Health, MaxHealth);

    if (Health <= 0.0f)
    {
        RespawnAtLastSpawnPoint();
    }
}

void AShixunCharacter::Heal(float HealAmount)
{
    if (HealAmount <= 0.0f) return;

    Health = FMath::Clamp(Health + HealAmount, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(Health, MaxHealth);

    UE_LOG(LogTemp, Log, TEXT("治疗: +%.1f, 当前血量: %.1f/%.1f"), HealAmount, Health, MaxHealth);
}

void AShixunCharacter::RespawnAtLastSpawnPoint()
{
    UE_LOG(LogTemp, Log, TEXT("玩家死亡，2秒后在重生点复活！"));

    if (Controller)
    {
        Controller->SetIgnoreMoveInput(true);
    }

    bIsRespawning = true;

    FTimerHandle RespawnTimerHandle;
    GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &AShixunCharacter::FinishRespawn, 2.0f, false);
}

void AShixunCharacter::FinishRespawn()
{
    Health = MaxHealth;
    OnHealthChanged.Broadcast(Health, MaxHealth);

    SetActorLocation(LastSpawnLocation, false, nullptr, ETeleportType::TeleportPhysics);
    SetActorRotation(LastSpawnRotation);
    if (Controller)
    {
        Controller->SetControlRotation(LastSpawnRotation);
        Controller->SetIgnoreMoveInput(false);
    }

    bIsRespawning = false;

    UE_LOG(LogTemp, Log, TEXT("重生完成！"));
}

void AShixunCharacter::StartTimeReverse()
{
    if (InventoryComponent && InventoryComponent->bInventoryOpen) return;
    if (CurrentState != EAbilityState::Default) return;
    if (TimeRewindCooldownRemaining > 0) return;

    UE_LOG(LogTemp, Warning, TEXT("!!! TimeReverse STARTED — check if F key was pressed or Blueprint called"));
    CurrentState = EAbilityState::TimeRewind;
    myTimeComponent->isTimeReversing = true;
    TimeReverseDelegate.Broadcast(true);
}

void AShixunCharacter::StopTimeReverse()
{
    if (InventoryComponent && InventoryComponent->bInventoryOpen) return;
    if (CurrentState != EAbilityState::TimeRewind) return;
    myTimeComponent->isTimeReversing = false;
    TimeReverseDelegate.Broadcast(false);
}

void AShixunCharacter::OnGrabPressed()
{
    if (InventoryComponent && InventoryComponent->bInventoryOpen) return;
    if (!GrabComponent) return;

    bIsGrabKeyHeld = true;
    if (CrosshairWidgetInstance && !CrosshairWidgetInstance->IsInViewport())
    {
        CrosshairWidgetInstance->AddToViewport();
    }
}

void AShixunCharacter::OnGrabReleased()
{
    bIsGrabKeyHeld = false;
    if (!GrabComponent) return;

    GrabComponent->ReleaseGrab();
    if (CrosshairWidgetInstance && CrosshairWidgetInstance->IsInViewport())
    {
        CrosshairWidgetInstance->RemoveFromViewport();
    }
    SetCrosshairColor(FLinearColor::White);
}

void AShixunCharacter::UpdateGrab()
{
    if (!GrabComponent) return;
    if (bIsGrabKeyHeld)
    {
        GrabComponent->StartGrab();
    }
}

void AShixunCharacter::SetCrosshairColor(const FLinearColor& Color)
{
    if (!CrosshairWidgetInstance) return;
    UImage* CrosshairImage = Cast<UImage>(CrosshairWidgetInstance->WidgetTree->FindWidget(FName("Image_1")));
    if (CrosshairImage)
    {
        CrosshairImage->SetColorAndOpacity(Color);
    }
}

void AShixunCharacter::OnGrabSuccess()
{
    SetCrosshairColor(FLinearColor::Green);
}

void AShixunCharacter::OnToggleInventory()
{
    if (InventoryComponent)
    {
        InventoryComponent->ToggleInventory();
    }
}

void AShixunCharacter::OnInteract()
{
    if (InventoryComponent && InventoryComponent->bInventoryOpen) return;
    if (!InventoryComponent) return;

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    FVector CamLoc;
    FRotator CamRot;
    PC->GetPlayerViewPoint(CamLoc, CamRot);

    FVector TraceEnd = CamLoc + CamRot.Vector() * 400.0f;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, TraceEnd, ECC_Visibility, Params))
    {
        AActor* HitActor = Hit.GetActor();
        if (!HitActor) return;

        UPickupComponent* PickupComp = HitActor->FindComponentByClass<UPickupComponent>();
        if (PickupComp && PickupComp->ItemID != EItemID::None)
        {
            if (InventoryComponent->AddItem(PickupComp->ItemID) != INDEX_NONE)
            {
                HitActor->Destroy();
            }
        }
    }
}

float AShixunCharacter::GetTimeRewindCooldownPercentage() const
{
    if (TimeRewindCooldown <= 0.0f) return 0.0f;
    return FMath::Clamp(TimeRewindCooldownRemaining / TimeRewindCooldown, 0.0f, 1.0f);
}

bool AShixunCharacter::IsTimeReversing() const
{
    if (!myTimeComponent) return false;
    return myTimeComponent->IsReversing();
}

void AShixunCharacter::OnRotateLeftPressed()
{
    if (InventoryComponent && InventoryComponent->bInventoryOpen) return;
    if (GrabComponent && GrabComponent->IsGrabbing())
    {
        GrabComponent->StartRotateLeft();
    }
}

void AShixunCharacter::OnRotateLeftReleased()
{
    if (InventoryComponent && InventoryComponent->bInventoryOpen) return;
    if (GrabComponent)
    {
        GrabComponent->StopRotateLeft();
    }
}

void AShixunCharacter::OnRotateRightPressed()
{
    if (InventoryComponent && InventoryComponent->bInventoryOpen) return;
    if (GrabComponent && GrabComponent->IsGrabbing())
    {
        GrabComponent->StartRotateRight();
    }
}

void AShixunCharacter::OnRotateRightReleased()
{
    if (InventoryComponent && InventoryComponent->bInventoryOpen) return;
    if (GrabComponent)
    {
        GrabComponent->StopRotateRight();
    }
}