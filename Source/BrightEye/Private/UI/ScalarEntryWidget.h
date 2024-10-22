// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Text/STextBlock.h"

// Delegate to handle slider value change
DECLARE_DELEGATE_OneParam(FOnScalarParamChanged, const float& /* Value */);

/**
 * Widget that allows users to adjust a scalar (float) value using a slider, 
 * with visual feedback via a progress bar and value text.
 */
class BRIGHTEYE_API SScalarEntryWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SScalarEntryWidget)
		: _Title(FText::FromString("Default Title")) // Default title value
		, _InitialProgress(0.0f)                    // Default progress value
		{}

	SLATE_ARGUMENT(FText, Title)
	SLATE_ARGUMENT(float, InitialProgress)
	SLATE_EVENT(FOnScalarParamChanged, OnValueChanged) // Delegate for value changes
SLATE_END_ARGS()

/** Constructs this widget with InArgs */
void Construct(const FArguments& InArgs);

private:
	TSharedPtr<STextBlock> Title;
	TSharedPtr<SProgressBar> ProgressBar;
	TSharedPtr<SSlider> Slider;
	TSharedPtr<STextBlock> ValueText;

	/** Callback when slider value changes */
	void OnSliderValueChanged(float NewValue) const;

	FOnScalarParamChanged OnValueChanged; // Store the delegate for value change

public:
	FOnScalarParamChanged& GetParamChangedSignature();

	void UpdateProgressManually(const float& InValue) const;
};
