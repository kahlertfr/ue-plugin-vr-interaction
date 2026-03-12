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

void UVRLogManager::AddMessageF(const TCHAR* Fmt, ...)
{
	va_list Args;
	va_start(Args, Fmt);
	AddMessage(FString::PrintfV(Fmt, Args));
	va_end(Args);
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
