// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#include "ControlPanel.h"
#include "Data/BrightEyeSettings.h"
#include "Data/ColorPicker.h"


void UBEControlPanel::InitializePanel()
{
	ColorPicker = NewObject<UBEColorPicker>(this);
}

void UBEControlPanel::NativeDestruct()
{
	Super::NativeDestruct();

	ColorPicker = nullptr;
}

void UBEControlPanel::CallOnSmoothRotationStateChanged() const
{
	if(OnDelayStateChanged.IsBound())
	{
		OnDelayStateChanged.Execute();
	}
}

bool UBEControlPanel::GetIsSmoothRotationEnabled()
{
	if(UBESettings::GetInstance())
	{
		return UBESettings::GetInstance()->bSmoothCameraRotation;
	}
	
	return false;
}

FOnSmoothRotationToggled& UBEControlPanel::GetOnDelayStateChanged() 
{
	return OnDelayStateChanged;
}
