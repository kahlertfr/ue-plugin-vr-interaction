#pragma once

#include "Components/ActorComponent.h"
#include "MenuTouchGateComponent.generated.h"

class UPrimitiveComponent;

/** One activation per contact, including contacts rejected during the cooldown. */
struct VRINTERACTION_API FMenuTouchGateState
{
    bool TryTouch(double Now, double Cooldown);
    void UpdateContact(bool bTouching, double Now, double ReleaseDelay);

    bool bLatched = false;
    double LastActivation = -TNumericLimits<double>::Max();
    double ClearSince = -1;
};

/** Added to menu icons; keeps existing finger interfaces unchanged for other controls. */
UCLASS(ClassGroup="VR", meta=(BlueprintSpawnableComponent))
class VRINTERACTION_API UMenuTouchGateComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UMenuTouchGateComponent();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Menu Touch", meta=(ClampMin="0.0"))
    float ReleaseDelay = 0.15f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Menu Touch", meta=(ClampMin="0.0"))
    float ToggleCooldown = 0.5f;

    /** Call before sending FingerCollision. Actors without this component pass through. */
    UFUNCTION(BlueprintCallable, Category="VR|Menu Touch")
    static bool ShouldDispatchFingerTouch(AActor* TouchedActor, UPrimitiveComponent* Finger,
        UPrimitiveComponent* TouchedComponent);

    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

private:
    bool IsAnyFingerTouching() const;
    FMenuTouchGateState State;
    TSet<TWeakObjectPtr<UPrimitiveComponent>> Fingers;
};
