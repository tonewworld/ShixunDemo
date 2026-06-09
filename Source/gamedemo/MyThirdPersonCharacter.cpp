#include "MyThirdPersonCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "GrabComponent.h"
#include "TimeComponent.h"
#include "Components/Image.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/UserWidget.h"

AMyThirdPersonCharacter::AMyThirdPersonCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // 创建弹簧臂组件
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);

    CameraBoom->TargetArmLength = 300.0f;
    CameraBoom->bDoCollisionTest = true;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bInheritPitch = true;
    CameraBoom->bInheritYaw = true;
    CameraBoom->bInheritRoll = false;

    // 创建相机
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));

    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->bOrientRotationToMovement = false;
        MoveComp->bUseControllerDesiredRotation = true;
        MoveComp->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
        MoveComp->JumpZVelocity = 600.0f;
        MoveComp->AirControl = 0.2f;
    }

    bUseControllerRotationYaw = true;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

    MaxHealth = 100.0f;
    Health = MaxHealth;

    myTimeComponent = CreateDefaultSubobject<UTimeComponent>(TEXT("myTimeComponent"));
    GrabComponent = CreateDefaultSubobject<UGrabComponent>(TEXT("GrabComponent"));
}

void AMyThirdPersonCharacter::BeginPlay()
{
    Super::BeginPlay();
    OnHealthChanged.Broadcast(Health, MaxHealth);

    if (CrosshairWidgetClass)
    {
        CrosshairWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetClass);
        SetCrosshairColor(FLinearColor::White);
    }
    if (GrabComponent)
    {
        GrabComponent->OnGrabSuccess.AddDynamic(this, &AMyThirdPersonCharacter::OnGrabSuccess);
    }
}

void AMyThirdPersonCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateGrab();
}

void AMyThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AMyThirdPersonCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMyThirdPersonCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &AMyThirdPersonCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AMyThirdPersonCharacter::LookUp);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
    PlayerInputComponent->BindAction("TestDamage", IE_Pressed, this, &AMyThirdPersonCharacter::TestDamage);
    PlayerInputComponent->BindAction("TimeReverse", IE_Pressed, this, &AMyThirdPersonCharacter::StartTimeReverse);
    PlayerInputComponent->BindAction("TimeReverse", IE_Released, this, &AMyThirdPersonCharacter::StopTimeReverse);
    PlayerInputComponent->BindAction("Grab", IE_Pressed, this, &AMyThirdPersonCharacter::OnGrabPressed);
    PlayerInputComponent->BindAction("Grab", IE_Released, this, &AMyThirdPersonCharacter::OnGrabReleased);

    // ========== 新增：绑定推/拉动作（需要在项目输入设置中手动添加 Push / Pull） ==========
    PlayerInputComponent->BindAction("Push", IE_Pressed, this, &AMyThirdPersonCharacter::OnPushPressed);
    PlayerInputComponent->BindAction("Push", IE_Released, this, &AMyThirdPersonCharacter::OnPushReleased);
    PlayerInputComponent->BindAction("Pull", IE_Pressed, this, &AMyThirdPersonCharacter::OnPullPressed);
    PlayerInputComponent->BindAction("Pull", IE_Released, this, &AMyThirdPersonCharacter::OnPullReleased);
}

void AMyThirdPersonCharacter::MoveForward(float Value)
{
    if (Controller && Value != 0.0f)
    {
        const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AMyThirdPersonCharacter::MoveRight(float Value)
{
    if (Controller && Value != 0.0f)
    {
        const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}

void AMyThirdPersonCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}

void AMyThirdPersonCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

void AMyThirdPersonCharacter::ApplyDamage(float DamageAmount)
{
    if (DamageAmount <= 0.0f) return;
    Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(Health, MaxHealth);
    if (Health <= 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s has died!"), *GetName());
    }
}

void AMyThirdPersonCharacter::TestDamage()
{
    ApplyDamage(10.0f);
    UE_LOG(LogTemp, Log, TEXT("TestDamage called. Current Health: %f / %f"), Health, MaxHealth);
}

void AMyThirdPersonCharacter::StartTimeReverse()
{
    myTimeComponent->isTimeReversing = true;
    TimeReverseDelegate.Broadcast(true);
}

void AMyThirdPersonCharacter::StopTimeReverse()
{
    myTimeComponent->isTimeReversing = false;
    TimeReverseDelegate.Broadcast(false);
}

void AMyThirdPersonCharacter::OnGrabPressed()
{
    if (CrosshairWidgetInstance && !CrosshairWidgetInstance->IsInViewport())
    {
        CrosshairWidgetInstance->AddToViewport();
    }
    bIsGrabKeyHeld = true;
}

void AMyThirdPersonCharacter::OnGrabReleased()
{
    if (CrosshairWidgetInstance && CrosshairWidgetInstance->IsInViewport())
    {
        CrosshairWidgetInstance->RemoveFromViewport();
    }
    bIsGrabKeyHeld = false;
    if (GrabComponent)
    {
        GrabComponent->ReleaseGrab();
    }
    SetCrosshairColor(FLinearColor::White);
}

void AMyThirdPersonCharacter::UpdateGrab()
{
    if (!GrabComponent) return;
    if (bIsGrabKeyHeld)
    {
        GrabComponent->TryGrab();
    }
}

void AMyThirdPersonCharacter::SetCrosshairColor(const FLinearColor& Color)
{
    if (!CrosshairWidgetInstance) return;
    UImage* CrosshairImage = Cast<UImage>(CrosshairWidgetInstance->WidgetTree->FindWidget(FName("Image_1")));
    if (CrosshairImage)
    {
        CrosshairImage->SetColorAndOpacity(Color);
    }
}

void AMyThirdPersonCharacter::OnGrabSuccess()
{
    SetCrosshairColor(FLinearColor::Green);
}

// ========== 新增：推/拉回调实现 ==========
void AMyThirdPersonCharacter::OnPushPressed()
{
    if (GrabComponent && GrabComponent->IsGrabbing())
    {
        GrabComponent->StartPush();
    }
}

void AMyThirdPersonCharacter::OnPushReleased()
{
    if (GrabComponent)
    {
        GrabComponent->StopPush();
    }
}

void AMyThirdPersonCharacter::OnPullPressed()
{
    if (GrabComponent && GrabComponent->IsGrabbing())
    {
        GrabComponent->StartPull();
    }
}

void AMyThirdPersonCharacter::OnPullReleased()
{
    if (GrabComponent)
    {
        GrabComponent->StopPull();
    }
}