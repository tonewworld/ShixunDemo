#include "ShixunCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "GrabComponent.h"
#include "TimeComponent.h"
#include "Components/Image.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/UserWidget.h"

AShixunCharacter::AShixunCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // 第一人称相机（在胶囊体中心，胶囊体碰撞防止穿墙）
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(RootComponent);
    FollowCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f));
    FollowCamera->bUsePawnControlRotation = true;

    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->bOrientRotationToMovement = false;
        MoveComp->bUseControllerDesiredRotation = true;
        MoveComp->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
        MoveComp->JumpZVelocity = 600.0f;
        MoveComp->AirControl = 0.2f;
    }

    bUseControllerRotationYaw = true;
    bUseControllerRotationPitch = true;
    bUseControllerRotationRoll = false;

    GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

    MaxHealth = 100.0f;
    Health = MaxHealth;

    myTimeComponent = CreateDefaultSubobject<UTimeComponent>(TEXT("myTimeComponent"));
    GrabComponent = CreateDefaultSubobject<UGrabComponent>(TEXT("GrabComponent"));
}

void AShixunCharacter::BeginPlay()
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
        GrabComponent->OnGrabSuccess.AddDynamic(this, &AShixunCharacter::OnGrabSuccess);
    }
}

void AShixunCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateGrab();
}

void AShixunCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AShixunCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AShixunCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &AShixunCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AShixunCharacter::LookUp);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
    PlayerInputComponent->BindAction("TestDamage", IE_Pressed, this, &AShixunCharacter::TestDamage);
    PlayerInputComponent->BindAction("TimeReverse", IE_Pressed, this, &AShixunCharacter::StartTimeReverse);
    PlayerInputComponent->BindAction("TimeReverse", IE_Released, this, &AShixunCharacter::StopTimeReverse);
    PlayerInputComponent->BindAction("Grab", IE_Pressed, this, &AShixunCharacter::OnGrabPressed);

    PlayerInputComponent->BindAction("Push", IE_Pressed, this, &AShixunCharacter::OnPushPressed);
    PlayerInputComponent->BindAction("Push", IE_Released, this, &AShixunCharacter::OnPushReleased);
    PlayerInputComponent->BindAction("Pull", IE_Pressed, this, &AShixunCharacter::OnPullPressed);
    PlayerInputComponent->BindAction("Pull", IE_Released, this, &AShixunCharacter::OnPullReleased);
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

void AShixunCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}

void AShixunCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

void AShixunCharacter::ApplyDamage(float DamageAmount)
{
    if (DamageAmount <= 0.0f) return;
    Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(Health, MaxHealth);
    if (Health <= 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s has died!"), *GetName());
    }
}

void AShixunCharacter::TestDamage()
{
    ApplyDamage(10.0f);
    UE_LOG(LogTemp, Log, TEXT("TestDamage called. Current Health: %f / %f"), Health, MaxHealth);
}

void AShixunCharacter::StartTimeReverse()
{
    myTimeComponent->isTimeReversing = true;
    TimeReverseDelegate.Broadcast(true);
}

void AShixunCharacter::StopTimeReverse()
{
    myTimeComponent->isTimeReversing = false;
    TimeReverseDelegate.Broadcast(false);
}

void AShixunCharacter::OnGrabPressed()
{
	if (!GrabComponent) return;

	// 按一下F切换：如果已抓住物体则释放，否则进入抓取模式
	if (GrabComponent->IsGrabbing())
	{
		GrabComponent->ReleaseGrab();
		bIsGrabKeyHeld = false;
		if (CrosshairWidgetInstance && CrosshairWidgetInstance->IsInViewport())
		{
			CrosshairWidgetInstance->RemoveFromViewport();
		}
		SetCrosshairColor(FLinearColor::White);
	}
	else
	{
		bIsGrabKeyHeld = true;
		if (CrosshairWidgetInstance && !CrosshairWidgetInstance->IsInViewport())
		{
			CrosshairWidgetInstance->AddToViewport();
		}
	}
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

float AShixunCharacter::GetTimeEnergyPercentage() const
{
    if (!myTimeComponent) return 0.0f;
    return myTimeComponent->GetEnergyPercentage();
}

bool AShixunCharacter::IsTimeReversing() const
{
    if (!myTimeComponent) return false;
    return myTimeComponent->IsReversing();
}

void AShixunCharacter::OnPushPressed()
{
    if (GrabComponent && GrabComponent->IsGrabbing())
    {
        GrabComponent->StartPush();
    }
}

void AShixunCharacter::OnPushReleased()
{
    if (GrabComponent)
    {
        GrabComponent->StopPush();
    }
}

void AShixunCharacter::OnPullPressed()
{
    if (GrabComponent && GrabComponent->IsGrabbing())
    {
        GrabComponent->StartPull();
    }
}

void AShixunCharacter::OnPullReleased()
{
    if (GrabComponent)
    {
        GrabComponent->StopPull();
    }
}
