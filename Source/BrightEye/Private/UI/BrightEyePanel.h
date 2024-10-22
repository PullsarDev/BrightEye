// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#pragma once

#include "CoreMinimal.h"
#include "CursorTrackerWidget.h"
#include "Widgets/SCompoundWidget.h"

struct FPanelFadeOutManager;
class UBEColorPicker;

DECLARE_DELEGATE(FOnSmoothRotationStateChangedSignature);
DECLARE_DELEGATE_OneParam(FOnScalarValueChangedSignature, const float& /* Value */);
DECLARE_DELEGATE(FOnPanelDragStartedSignature)
DECLARE_DELEGATE_OneParam(FOnPanelDragFinishedSignature,const FVector2D& /* Drop Location */)


class SScalarEntryWidget;

/**
 * SBrightEyePanel is a custom Slate widget used for managing the UI panel of the BrightEye.
 * It handles brightness, radius, distance, and other light-related parameters, 
 * with support for drag-and-drop, color picking, and smooth rotation toggling.
 */
class BRIGHTEYE_API SBrightEyePanel final : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SBrightEyePanel){}
	SLATE_EVENT(FOnPanelDragStartedSignature, OnPanelDragStarted)
	SLATE_EVENT(FOnPanelDragFinishedSignature, OnPanelDragFinished)
	SLATE_EVENT(FOnScalarValueChangedSignature, OnBrightnessChanged)
	SLATE_EVENT(FOnScalarValueChangedSignature, OnRadiusChanged)
	SLATE_EVENT(FOnScalarValueChangedSignature, OnDistanceChanged)
	SLATE_EVENT(FOnCoordChangedSignature, OnCoordsChanged)
	SLATE_EVENT(FOnSmoothRotationStateChangedSignature, OnSmoothRotationStateChanged)
SLATE_END_ARGS()


	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	
	FReply OnSmoothRotationButtonClicked() const;
	FReply OnColorButtonClicked() const;
	
	bool InitializeTheColorPicker();
	UBEColorPicker* GetColorPicker();
	
private:
	TSharedPtr<SScalarEntryWidget> BrightnessEntry;
	TSharedPtr<SScalarEntryWidget> RadiusEntry;
	TSharedPtr<SScalarEntryWidget> DistanceEntry;
	TSharedPtr<SImage> SmoothRotationImage;
	
	TSharedPtr<SWidget> ScalarParamBox;
	TSharedPtr<SWidget> LightCoordOverlay;
	
	TSharedPtr<SCursorTrackerWidget> CursorTrackerWidget;

	FOnScalarValueChangedSignature OnBrightnessChangedSignature;
	FOnScalarValueChangedSignature OnRadiusChangedSignature;
	FOnScalarValueChangedSignature OnDistanceChangedSignature;

public:
	FOnSmoothRotationStateChangedSignature OnSmoothRotationStateChanged;

	void RefreshPanel() const;
	void RefreshBrightness() const;
	void RefreshRadius() const;
	void RefreshDistance() const;
	void RefreshCameraRotationState() const;
	void RefreshColor() const;
	void ChangeHidePanelWhenIdle(bool InHidePanelWhenIdle);
	void UpdateViewCoords() const;
	
private:
	static const FSlateBrush* GetSmoothRotationButtonImage();
	
	bool bIsOnTheScreen = false;

	bool bHidePanelWhenIdle = false;

	TObjectPtr<UBEColorPicker> ColorPicker = nullptr;

#pragma region Drag&Drop
	
	FOnPanelDragStartedSignature OnPanelDragStartedSignature;
	FOnPanelDragFinishedSignature OnPanelDragFinishedSignature;
	
	virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	void FinishDraggingWidget(const FVector2D InLocation) const;
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;

#pragma endregion Drag&Drop

private:
	//Fadeout Management
	FPanelFadeOutManager* FadeOutManager = nullptr;
	
	void CreateFadeOutManager();

public:
	virtual ~SBrightEyePanel() override;
	
};
