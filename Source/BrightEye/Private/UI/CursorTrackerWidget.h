// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#pragma once

#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"

DECLARE_DELEGATE_OneParam(FOnCoordChangedSignature,const FVector2D& OutCoord)

/**
 * SCursorTrackerWidget is a custom Slate widget designed to track and update the position of the cursor,
 * providing coordinate changes via a delegate and supporting mouse interactions like dragging and snapping.
 */
class SCursorTrackerWidget : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(SCursorTrackerWidget) {}
		SLATE_ARGUMENT(FVector2D,InitialCoords)
		SLATE_EVENT(FOnCoordChangedSignature, OnCoordChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	// FSlateWidget overrides
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	
private:
	FVector2D LightPosition;
	bool bIsDragging = false;
	float SnapThreshold = 0.05f; 
	FVector2D Coordinates;   
	FOnCoordChangedSignature OnCoordChangedSignature;
	
	void CalculateCoordinates(const FVector2D& Position, const FVector2D& WidgetSize);
	FVector2D ApplySnapToAxes(const FVector2D& Position, const FVector2D& WidgetSize) const;

	static FVector2D CalculateLightPosition(const FVector2D& PolarCoordinates, const FVector2D& InWidgetSize);

public:
	void UpdateViewCoords(const FVector2D& InCoords);
};

