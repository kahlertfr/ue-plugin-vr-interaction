#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "VRGrabSelection.generated.h"

class UPrimitiveComponent;
class USceneComponent;

UCLASS()
class VRINTERACTION_API UVRGrabSelection : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    /** Select only grabs belonging to collision geometry within reach, never sibling view pivots.
     * Fallback retains the pawn's earlier special-case probe selection when nothing is touched. */
    UFUNCTION(BlueprintPure, Category="VR|Grab", meta=(DeterminesOutputType="GrabComponentClass"))
    static USceneComponent* FindGrabComponentNearGrip(UPrimitiveComponent* Grip, float Radius,
        TSubclassOf<USceneComponent> GrabComponentClass, USceneComponent* Fallback);

    /** Resolve the nearest attachment ancestor with a directly attached grab component. */
    static USceneComponent* ResolveGrabComponent(UPrimitiveComponent* Collider, UClass* GrabComponentClass);
};
