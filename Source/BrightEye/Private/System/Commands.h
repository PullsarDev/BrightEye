// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev

#pragma once

#include "CoreMinimal.h"

/**
 * Handles the Bright Eye command bindings for toggling the light and control panel visibility.
 * Manages the user-defined commands within the Bright Eye tool, ensuring proper action bindings.
 */
class FBECommands : public TCommands<FBECommands>
{
public:
	FBECommands() : TCommands<FBECommands>(
		TEXT("Bright Eye Commands"),
		FText::FromString(TEXT("Bright Eye Actions")),
		NAME_None,
		FAppStyle::GetAppStyleSetName()
	) {}

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> ToggleBrightEye;
	TSharedPtr<FUICommandInfo> ToggleControlPanel;
};