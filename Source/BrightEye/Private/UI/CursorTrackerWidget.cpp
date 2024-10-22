// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#include "CursorTrackerWidget.h"
#include "System/BrightEyeStyle.h"  

const FVector2D DefaultWidgetSize(214.0f, 82.0f);

void SCursorTrackerWidget::Construct(const FArguments& InArgs)
{
    LightPosition = CalculateLightPosition(InArgs._InitialCoords,DefaultWidgetSize);
    OnCoordChangedSignature = InArgs._OnCoordChanged;
    bIsDragging = false;
}

int32 SCursorTrackerWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    constexpr float CircleRadius = 10.0f;
    const FVector2D CircleSize = FVector2D(CircleRadius * 2.0f, CircleRadius * 2.0f);
        
    FVector2D AdjustedPosition = LightPosition - FVector2D(CircleRadius, CircleRadius);

    const FSlateBrush* CircleBrush = FSlateIcon(FBrightEyeStyle::GetToolStyleName(), "BrightEye.Target").GetIcon();

    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId + 1,
        AllottedGeometry.ToPaintGeometry(CircleSize, FSlateLayoutTransform(AdjustedPosition)),
        CircleBrush 
    );
    return LayerId + 2;
}

FReply SCursorTrackerWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        bIsDragging = true;
        LightPosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
        return FReply::Handled();
    }
    return FReply::Unhandled();
}

FReply SCursorTrackerWidget::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        bIsDragging = false;
        return FReply::Handled();
    }
    return FReply::Unhandled();
}

FReply SCursorTrackerWidget::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (bIsDragging)
    {
        FVector2D LocalPosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());

        LightPosition = ApplySnapToAxes(LocalPosition, MyGeometry.GetLocalSize());

        CalculateCoordinates(LightPosition, MyGeometry.GetLocalSize());

        if(OnCoordChangedSignature.IsBound())
        {
            OnCoordChangedSignature.Execute(Coordinates);
        }

        return FReply::Handled();
    }
    return FReply::Unhandled();
}

void SCursorTrackerWidget::CalculateCoordinates(const FVector2D& Position, const FVector2D& WidgetSize)
{
    Coordinates.X = (Position.X / (WidgetSize.X * 0.5f)) - 1.0f;
    Coordinates.Y = 1.0f - (Position.Y / (WidgetSize.Y * 0.5f));
}

FVector2D SCursorTrackerWidget::CalculateLightPosition(const FVector2D& PolarCoordinates, const FVector2D& InWidgetSize)
{
    FVector2D LocalPosition;
    LocalPosition.X = (PolarCoordinates.X + 1.0f) * 0.5f * InWidgetSize.X;
    LocalPosition.Y = (1.0f - PolarCoordinates.Y) * 0.5f * InWidgetSize.Y;
    return LocalPosition;
}


FVector2D SCursorTrackerWidget::ApplySnapToAxes(const FVector2D& Position, const FVector2D& WidgetSize) const
{
    FVector2D SnappedPosition = Position;

    if (FMath::Abs(Position.X - (WidgetSize.X * 0.5f)) < SnapThreshold * WidgetSize.X)
    {
        SnappedPosition.X = WidgetSize.X * 0.5f; 
    }

    if (FMath::Abs(Position.Y - (WidgetSize.Y * 0.5f)) < SnapThreshold * WidgetSize.Y)
    {
        SnappedPosition.Y = WidgetSize.Y * 0.5f;
    }

    return SnappedPosition;
}

FVector2D SCursorTrackerWidget::ComputeDesiredSize(float) const
{
    return FVector2D(100.f, 100.f);
}

void SCursorTrackerWidget::UpdateViewCoords(const FVector2D& InCoords)
{
    LightPosition = CalculateLightPosition(InCoords,DefaultWidgetSize);
}


