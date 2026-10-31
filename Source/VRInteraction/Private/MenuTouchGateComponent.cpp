#include "MenuTouchGateComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

bool FMenuTouchGateState::TryTouch(double Now, double Cooldown)
{
    const bool bAccept = !bLatched && Now - LastActivation >= Cooldown;
    // A new contact consumes the touch even during cooldown. Holding it must
    // never produce a delayed activation once the cooldown expires.
    bLatched = true;
    ClearSince = -1;
    if (bAccept) LastActivation = Now;
    return bAccept;
}

void FMenuTouchGateState::UpdateContact(bool bTouching, double Now, double ReleaseDelay)
{
    if (!bLatched) return;
    if (bTouching) ClearSince = -1;
    else if (ClearSince < 0) ClearSince = Now;
    if (!bTouching && Now - ClearSince >= ReleaseDelay)
    {
        bLatched = false;
        ClearSince = -1;
    }
}

UMenuTouchGateComponent::UMenuTouchGateComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
    PrimaryComponentTick.TickGroup = TG_PostPhysics;
}

bool UMenuTouchGateComponent::ShouldDispatchFingerTouch(AActor* TouchedActor,
    UPrimitiveComponent* Finger, UPrimitiveComponent* TouchedComponent)
{
    if (!IsValid(TouchedActor)) return false;
    auto* Gate = TouchedActor->FindComponentByClass<UMenuTouchGateComponent>();
    if (!Gate) return true;
    if (!IsValid(Finger) || !IsValid(TouchedComponent) ||
        TouchedComponent->GetOwner() != TouchedActor || !Gate->GetWorld()) return false;

    Gate->Fingers.Add(Finger);
    Gate->SetComponentTickEnabled(true);
    return Gate->State.TryTouch(Gate->GetWorld()->GetTimeSeconds(), Gate->ToggleCooldown);
}

bool UMenuTouchGateComponent::IsAnyFingerTouching() const
{
    for (const auto& Finger : Fingers)
    {
        if (!Finger.IsValid() || !Finger->IsQueryCollisionEnabled()) continue;
        TArray<UPrimitiveComponent*> Overlaps;
        Finger->GetOverlappingComponents(Overlaps);
        for (auto* Other : Overlaps)
            if (IsValid(Other) && Other->GetOwner() == GetOwner() && Other->IsQueryCollisionEnabled())
                return true;
    }
    return false;
}

void UMenuTouchGateComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    State.UpdateContact(IsAnyFingerTouching(), GetWorld()->GetTimeSeconds(), ReleaseDelay);
    if (!State.bLatched)
    {
        Fingers.Empty();
        SetComponentTickEnabled(false);
    }
}
