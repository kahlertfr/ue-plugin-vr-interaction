// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VRLogManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVRLogUpdated);

/**
 * Game-instance subsystem that collects log messages from anywhere in the codebase
 * and notifies interested listeners (e.g. AVRLogDisplayActor) whenever the log changes.
 *
 * Access from C++:
 *   UVRLogManager* LogManager = GameInstance->GetSubsystem<UVRLogManager>();
 *
 * Access from Blueprint:
 *   Get Game Instance → Get Subsystem (VRLogManager)
 */
UCLASS()
class VRINTERACTION_API UVRLogManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Fired whenever a message is added or the log is cleared. */
	UPROPERTY(BlueprintAssignable, Category = "VR Log")
	FOnVRLogUpdated OnLogUpdated;

	/**
	 * Append a message to the log.
	 * Older messages are dropped when MaxStoredMessages is exceeded.
	 */
	UFUNCTION(BlueprintCallable, Category = "VR Log")
	void AddMessage(const FString& Message);

	/** Remove all stored messages and fire OnLogUpdated. */
	UFUNCTION(BlueprintCallable, Category = "VR Log")
	void ClearMessages();

	/** Read-only access to all currently stored messages (newest last). */
	UFUNCTION(BlueprintCallable, Category = "VR Log")
	const TArray<FString>& GetMessages() const;

	/**
	 * Maximum number of messages kept in memory.
	 * When the limit is reached the oldest message is removed before adding the new one.
	 * Default: 100.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Log", meta = (ClampMin = "1"))
	int32 MaxStoredMessages = 100;

private:
	TArray<FString> Messages;
};
