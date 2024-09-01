// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ControlPanel.generated.h"

class UBEScalarEntry;
class UBEColorPicker;

DECLARE_DELEGATE(FOnSmoothRotationToggled);

/**
 * Manages the control panel UI for the Bright Eye tool, allowing users to adjust brightness, distance, radius, and color.
 * This panel also includes an option to toggle smooth camera rotation and handles interaction with UI widgets such as sliders and color pickers.
 */

UCLASS()
class BRIGHTEYE_API UBEControlPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializePanel();
	
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBEScalarEntry> BrightnessEntry;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBEScalarEntry> DistanceEntry;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBEScalarEntry> RadiusEntry;

	UPROPERTY()
	TObjectPtr<UBEColorPicker> ColorPicker;

	UPROPERTY(BlueprintReadWrite, Category = "Bright Eye UI")
	bool bHidePanelWhenIdle = false;

private:
	bool bIsOnTheScreen = false;
	FOnSmoothRotationToggled OnDelayStateChanged;

protected:
	UFUNCTION(BlueprintCallable, Category = "Bright Eye UI")
	void CallOnSmoothRotationStateChanged() const;

	UFUNCTION(BlueprintCallable, Category = "Bright Eye UI")
	bool GetIsSmoothRotationEnabled();

public:
	UBEScalarEntry* GetBrightnessEntry() const { return BrightnessEntry; }
	UBEScalarEntry* GetDistanceEntry() const { return DistanceEntry; }
	UBEScalarEntry* GetRadiusEntry() const { return RadiusEntry; }
	
	UFUNCTION(BlueprintCallable, Category = "Bright Eye UI")
	UBEColorPicker* GetColorPicker() const { return ColorPicker;}
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Bright Eye UI")
	void ChangeHidePanelWhenIdle(bool InInitialValue);

	void SetIsOnTheScreen(bool bInIsOnTheScreen) { bIsOnTheScreen = bInIsOnTheScreen;}
	bool GetIsOnTheScreen() const { return bIsOnTheScreen;}

	UFUNCTION(BlueprintImplementableEvent)
	void ReInitializeCameraRotationState();
	
	UFUNCTION(BlueprintImplementableEvent)
	void SmoothCameraRotationStateChanged();

	FOnSmoothRotationToggled& GetOnDelayStateChanged();
	
};
