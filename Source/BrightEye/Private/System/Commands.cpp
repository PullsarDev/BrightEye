// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#include "Commands.h"

void FBECommands::RegisterCommands()
{
	{
		const FText Label = FText::FromString(TEXT("Toggle Bright Eye"));
		const FText Desc = FText::FromString(TEXT("Toggles the BrightEye"));

		FUICommandInfo::MakeCommandInfo(
			this->AsShared(),
			ToggleBrightEye,
			"ToggleBrightEye",
			Label,
			Desc,
			FSlateIcon(),
			EUserInterfaceActionType::Button,
			FInputChord(EKeys::T));
	}
	{
		const FText Label = FText::FromString(TEXT("Aim Bright Eye"));
		const FText Desc = FText::FromString(TEXT("Directs the BrightEye light towards the mouse cursor while held."));

		FUICommandInfo::MakeCommandInfo(
			this->AsShared(),
			AimBrightEye,
			"AimBrightEye",
			Label,
			Desc,
			FSlateIcon(),
			EUserInterfaceActionType::Button,
			FInputChord(EKeys::F, EModifierKey::Control));
	}
	{
		const FText Label = FText::FromString(TEXT("Toggle Control Panel"));
		const FText Desc = FText::FromString(TEXT("Toggles the BrightEye control panel"));

		FUICommandInfo::MakeCommandInfo(
			this->AsShared(),
			ToggleControlPanel,
			"ToggleBrightEyeControlPanel",
			Label,
			Desc,
			FSlateIcon(),
			EUserInterfaceActionType::Button,
			FInputChord(EKeys::T, EModifierKey::Control));
	}
}