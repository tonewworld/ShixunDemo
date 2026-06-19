#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OfferingAltar.generated.h"

class UBoxComponent;
class UOfferingWidget;
class USoundBase;

UCLASS()
class GAMEDEMO_API AOfferingAltar : public AActor
{
    GENERATED_BODY()

public:
    AOfferingAltar();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UBoxComponent* TriggerBox;

    UPROPERTY(EditAnywhere, Category = "Offering")
    AActor* StoneA;

    UPROPERTY(EditAnywhere, Category = "Offering")
    AActor* StoneB;

    UPROPERTY(EditAnywhere, Category = "Offering")
    FVector StoneAOffset = FVector(300, 0, 0);

    UPROPERTY(EditAnywhere, Category = "Offering")
    FVector StoneBOffset = FVector(-300, 0, 0);

    UPROPERTY(EditAnywhere, Category = "Offering")
    float StoneMoveDuration = 2.0f;

    UPROPERTY(EditAnywhere, Category = "Offering")
    USoundBase* OfferSound;

    UFUNCTION()
    void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UFUNCTION()
    void OnOfferComplete();

    UFUNCTION()
    void DestroySelf();

private:
    UPROPERTY()
    UOfferingWidget* ActiveWidget;

    FVector StoneAStart;
    FVector StoneBStart;
    FVector StoneATarget;
    FVector StoneBTarget;
    float MoveElapsed = 0.0f;
    bool bIsMovingStones = false;
    FTimerHandle DestroyTimer;
};