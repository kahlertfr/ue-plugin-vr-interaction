#if WITH_DEV_AUTOMATION_TESTS
#include "MenuTouchGateComponent.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMenuTouchGateTiming, "TrajectoryBLab.VR.MenuTouchGate.Timing",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMenuTouchGateTiming::RunTest(const FString& Parameters)
{
    FMenuTouchGateState Gate;
    TestTrue(TEXT("First touch activates immediately"), Gate.TryTouch(0, 0.5));
    TestFalse(TEXT("Duplicate mesh/finger callback is blocked"), Gate.TryTouch(0, 0.5));
    Gate.UpdateContact(true, 2, 0.15);
    TestFalse(TEXT("Held touch cannot toggle after cooldown"), Gate.TryTouch(2, 0.5));
    Gate.UpdateContact(false, 2.01, 0.15);
    Gate.UpdateContact(false, 2.10, 0.15);
    TestTrue(TEXT("Short tracking dropout stays latched"), Gate.bLatched);
    Gate.UpdateContact(true, 2.11, 0.15);
    Gate.UpdateContact(false, 2.12, 0.15);
    Gate.UpdateContact(false, 2.26, 0.15);
    TestTrue(TEXT("Returning contact resets release interval"), Gate.bLatched);
    Gate.UpdateContact(false, 2.28, 0.15);
    TestFalse(TEXT("Stable release rearms"), Gate.bLatched);
    TestTrue(TEXT("Deliberate new touch toggles"), Gate.TryTouch(2.30, 0.5));
    Gate.UpdateContact(false, 2.31, 0.15);
    Gate.UpdateContact(false, 2.47, 0.15);
    TestFalse(TEXT("New touch inside cooldown is rejected"), Gate.TryTouch(2.48, 0.5));
    Gate.UpdateContact(true, 3, 0.15);
    TestFalse(TEXT("Rejected held touch never activates later"), Gate.TryTouch(3, 0.5));
    Gate.UpdateContact(false, 3.01, 0.15);
    Gate.UpdateContact(false, 3.17, 0.15);
    TestTrue(TEXT("Release and retouch after cooldown activates"), Gate.TryTouch(3.18, 0.5));
    FMenuTouchGateState OtherIcon;
    TestTrue(TEXT("Each icon has independent state"), OtherIcon.TryTouch(3.18, 0.5));
    return true;
}
#endif
