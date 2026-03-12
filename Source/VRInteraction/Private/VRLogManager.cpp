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
	constexpr int32 MaxMessageLength = 4096;
	TCHAR Buffer[MaxMessageLength];
	va_list Args;
	va_start(Args, Fmt);
	FCString::GetVarArgs(Buffer, MaxMessageLength, Fmt, Args);
	va_end(Args);
	AddMessage(FString(Buffer));
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
