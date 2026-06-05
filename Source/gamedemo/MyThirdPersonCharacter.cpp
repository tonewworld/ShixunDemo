#include "MyThirdPersonCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"

AMyThirdPersonCharacter::AMyThirdPersonCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // 创建弹簧臂组件
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);

    // 相机随鼠标旋转
    CameraBoom->TargetArmLength = 300.0f;
    CameraBoom->bDoCollisionTest = true;
    CameraBoom->bUsePawnControlRotation = true;   // 相机跟随鼠标
    CameraBoom->bInheritPitch = true;
    CameraBoom->bInheritYaw = true;
    CameraBoom->bInheritRoll = false;

    // 创建相机
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));

    // ========== 角色移动设置 ==========
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->bOrientRotationToMovement = false;          // 不自动转向移动方向
        MoveComp->bUseControllerDesiredRotation = true;       // 角色朝向控制器（鼠标）方向
        MoveComp->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
        MoveComp->JumpZVelocity = 600.0f;
        MoveComp->AirControl = 0.2f;
    }

    // ========== 角色模型跟随鼠标方向旋转 ==========
    // 角色的Yaw（水平旋转）跟随鼠标
    bUseControllerRotationYaw = true;
    // 角色的Pitch和Roll不跟随鼠标
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    // ========== 修正模型的初始朝向 ==========
    GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

    // 血量初始化
    MaxHealth = 100.0f;
    Health = MaxHealth;
}

void AMyThirdPersonCharacter::BeginPlay()
{
    Super::BeginPlay();
    OnHealthChanged.Broadcast(Health, MaxHealth);
}

void AMyThirdPersonCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AMyThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // 移动轴
    PlayerInputComponent->BindAxis("MoveForward", this, &AMyThirdPersonCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMyThirdPersonCharacter::MoveRight);

    // 鼠标/手柄视角控制轴
    PlayerInputComponent->BindAxis("Turn", this, &AMyThirdPersonCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AMyThirdPersonCharacter::LookUp);

    // 跳跃
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    // 测试伤害
    PlayerInputComponent->BindAction("TestDamage", IE_Pressed, this, &AMyThirdPersonCharacter::TestDamage);
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

// 实现 Turn
void AMyThirdPersonCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}

// 实现 LookUp
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