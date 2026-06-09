// MagneticInteractable.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MagneticInteractable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UMagneticInteractable : public UInterface
{
    GENERATED_BODY()
};

class GAMEDEMO_API IMagneticInteractable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "MagneticInteract")
        void OnGrabbed(AActor* Grabber);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "MagneticInteract")
        void OnMagneticRelease();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "MagneticInteract")
        void ApplyMagneticForce(FVector Force);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "MagneticInteract")
        bool IsGrabbedBy(AActor* Grabber) const;
};