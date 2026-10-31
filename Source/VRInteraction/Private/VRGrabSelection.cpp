#include "VRGrabSelection.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "CollisionQueryParams.h"

USceneComponent* UVRGrabSelection::ResolveGrabComponent(UPrimitiveComponent* Collider, UClass* GrabClass)
{
    if (!IsValid(Collider) || !GrabClass) return nullptr;
    for (USceneComponent* Parent = Collider; Parent; Parent = Parent->GetAttachParent())
    {
        for (USceneComponent* Child : Parent->GetAttachChildren())
            if (IsValid(Child) && Child->GetOwner() == Collider->GetOwner() && Child->IsA(GrabClass))
                return Child;
    }
    return nullptr;
}

USceneComponent* UVRGrabSelection::FindGrabComponentNearGrip(UPrimitiveComponent* Grip, float Radius,
    TSubclassOf<USceneComponent> GrabComponentClass, USceneComponent* Fallback)
{
    if (!IsValid(Grip) || !Grip->GetWorld() || !GrabComponentClass || !FMath::IsFinite(Radius) || Radius <= 0)
        return Fallback;
    const FVector Position = Grip->GetComponentLocation();
    TArray<FOverlapResult> Overlaps;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(VRGrabSelection), false, Grip->GetOwner());
    Grip->GetWorld()->OverlapMultiByObjectType(Overlaps, Position, FQuat::Identity,
        FCollisionObjectQueryParams(ECC_PhysicsBody), FCollisionShape::MakeSphere(Radius), Params);
    USceneComponent* Best = nullptr;
    double BestDistance = TNumericLimits<double>::Max();
    double BestCenterDistance = TNumericLimits<double>::Max();
    for (const FOverlapResult& Overlap : Overlaps)
    {
        UPrimitiveComponent* Collider = Overlap.GetComponent();
        USceneComponent* Grab = ResolveGrabComponent(Collider, GrabComponentClass);
        if (!Grab) continue;
        FVector ClosestPoint;
        const double Distance = Collider->GetClosestPointOnCollision(Position, ClosestPoint);
        if (Distance < 0 || Distance > Radius) continue;
        // Physics reports zero for points inside a shape. Use geometry centers only to
        // break ties between overlapping shapes, never the independent grab pivots.
        const double CenterDistance = FVector::DistSquared(Position, Collider->Bounds.Origin);
        if (Distance < BestDistance || (Distance == BestDistance &&
            (CenterDistance < BestCenterDistance || (CenterDistance == BestCenterDistance &&
                (!Best || Grab->GetPathName() < Best->GetPathName())))))
        {
            Best = Grab;
            BestDistance = Distance;
            BestCenterDistance = CenterDistance;
        }
    }
    return Best ? Best : Fallback;
}
