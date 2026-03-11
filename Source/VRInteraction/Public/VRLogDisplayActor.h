// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VRLogDisplayActor.generated.h"

/**
 * Abstract C++ base actor for displaying VR log messages inside the virtual scene.
 *
 * Extend this class in a Blueprint to add the visual representation (e.g. a
 * TextRenderComponent or a WidgetComponent containing a TextBlock).
 * Implement the BlueprintImplementableEvent OnLogTextUpdated to pass the
 * formatted text to your text component.
 *
 * The actor subscribes to UVRLogManager::OnLogUpdated automatically in BeginPlay
 * and calls UpdateLogDisplay whenever the log changes.
 *
 * Setup overview:
 *   1. Place an instance of your Blueprint in the level.
 *   2. Set MaxDisplayLines and MaxLineWidth in the Details panel (or keep defaults).
 *   3. Implement the OnLogTextUpdated event in your Blueprint and forward NewText
 *      to your TextRenderComponent / Widget TextBlock.
 *   4. From any C++ class or Blueprint call
 *        UVRLogManager* Log = GetGameInstance()->GetSubsystem<UVRLogManager>();
 *        Log->AddMessage("Hello VR!");
 *      to push messages that will appear on the display.
 */
UCLASS(Abstract, Blueprintable, ClassGroup = "VR Interaction")
class VRINTERACTION_API AVRLogDisplayActor : public AActor
{
	GENERATED_BODY()

public:
	AVRLogDisplayActor();

	/** Maximum number of lines shown on the display. Default: 10. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Log Display", meta = (ClampMin = "1"))
	int32 MaxDisplayLines = 10;

	/**
	 * Maximum number of characters per line.
	 * Lines longer than this value are word-wrapped.
	 * Default: 40.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Log Display", meta = (ClampMin = "1"))
	int32 MaxLineWidth = 40;

	/**
	 * Pull the latest messages from UVRLogManager, format them and call
	 * OnLogTextUpdated with the result.
	 * Can also be called manually from Blueprint at any time.
	 */
	UFUNCTION(BlueprintCallable, Category = "VR Log Display")
	void UpdateLogDisplay();

	/**
	 * Format an array of log messages so that:
	 *   - each message is word-wrapped to fit within InMaxLineWidth characters, and
	 *   - only the last InMaxDisplayLines lines are kept (newest at the bottom).
	 *
	 * Can be called as a pure helper from Blueprint without an actor instance.
	 */
	UFUNCTION(BlueprintCallable, Category = "VR Log Display")
	static FText FormatMessages(const TArray<FString>& Messages, int32 InMaxDisplayLines, int32 InMaxLineWidth);

	/**
	 * Called whenever the formatted display text changes.
	 * Implement this event in your Blueprint to forward NewText to your
	 * text component (TextRenderComponent, Widget TextBlock, etc.).
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "VR Log Display")
	void OnLogTextUpdated(const FText& NewText);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void HandleLogUpdated();

	/** Word-wrap a single string to at most InMaxLineWidth characters per line. */
	static TArray<FString> WordWrapLine(const FString& Input, int32 InMaxLineWidth);
};
