// Copyright Epic Games, Inc. All Rights Reserved.

#include "VRLogDisplayActor.h"
#include "VRLogManager.h"

AVRLogDisplayActor::AVRLogDisplayActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void AVRLogDisplayActor::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UVRLogManager* LogManager = GI->GetSubsystem<UVRLogManager>())
		{
			LogManager->OnLogUpdated.AddDynamic(this, &AVRLogDisplayActor::HandleLogUpdated);
			// Show whatever is already in the log when the actor spawns.
			UpdateLogDisplay();
		}
	}
}

void AVRLogDisplayActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UVRLogManager* LogManager = GI->GetSubsystem<UVRLogManager>())
		{
			LogManager->OnLogUpdated.RemoveDynamic(this, &AVRLogDisplayActor::HandleLogUpdated);
		}
	}

	Super::EndPlay(EndPlayReason);
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void AVRLogDisplayActor::UpdateLogDisplay()
{
	TArray<FString> Messages;
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UVRLogManager* LogManager = GI->GetSubsystem<UVRLogManager>())
		{
			Messages = LogManager->GetMessages();
		}
	}

	const FText FormattedText = FormatMessages(Messages, MaxDisplayLines, MaxLineWidth);
	OnLogTextUpdated(FormattedText);
}

FText AVRLogDisplayActor::FormatMessages(const TArray<FString>& Messages, int32 InMaxDisplayLines, int32 InMaxLineWidth)
{
	// 1. Word-wrap every message individually.
	TArray<FString> AllLines;
	for (const FString& Msg : Messages)
	{
		TArray<FString> Wrapped = WordWrapLine(Msg, InMaxLineWidth);
		AllLines.Append(Wrapped);
	}

	// 2. Keep only the last InMaxDisplayLines lines so the display is never overflowed.
	if (InMaxDisplayLines > 0 && AllLines.Num() > InMaxDisplayLines)
	{
		AllLines.RemoveAt(0, AllLines.Num() - InMaxDisplayLines);
	}

	return FText::FromString(FString::Join(AllLines, TEXT("\n")));
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

void AVRLogDisplayActor::HandleLogUpdated()
{
	UpdateLogDisplay();
}

TArray<FString> AVRLogDisplayActor::WordWrapLine(const FString& Input, int32 InMaxLineWidth)
{
	TArray<FString> Result;

	if (InMaxLineWidth <= 0)
	{
		Result.Add(Input);
		return Result;
	}

	// Split into individual words, preserving existing newlines as hard breaks.
	// Culling empty entries means consecutive or trailing newlines produce no extra blank lines.
	TArray<FString> HardLines;
	Input.ParseIntoArray(HardLines, TEXT("\n"), true);

	for (const FString& HardLine : HardLines)
	{
		TArray<FString> Words;
		HardLine.ParseIntoArrayWS(Words);

		FString CurrentLine;
		for (const FString& Word : Words)
		{
			if (CurrentLine.IsEmpty())
			{
				// A single word that is longer than the limit is placed on its own line as-is.
				CurrentLine = Word;
			}
			else if (CurrentLine.Len() + 1 + Word.Len() <= InMaxLineWidth)
			{
				CurrentLine += TEXT(" ") + Word;
			}
			else
			{
				Result.Add(CurrentLine);
				CurrentLine = Word;
			}
		}

		if (!CurrentLine.IsEmpty())
		{
			Result.Add(CurrentLine);
		}
	}

	return Result;
}
