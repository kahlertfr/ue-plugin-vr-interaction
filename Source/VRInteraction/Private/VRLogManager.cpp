// Copyright Epic Games, Inc. All Rights Reserved.

#include "VRLogManager.h"

void UVRLogManager::AddMessage(const FString& Message)
{
	if (MaxStoredMessages > 0 && Messages.Num() >= MaxStoredMessages)
	{
		Messages.RemoveAt(0);
	}
	Messages.Add(Message);
	OnLogUpdated.Broadcast();
}

void UVRLogManager::ClearMessages()
{
	Messages.Empty();
	OnLogUpdated.Broadcast();
}

const TArray<FString>& UVRLogManager::GetMessages() const
{
	return Messages;
}
