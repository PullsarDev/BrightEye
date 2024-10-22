// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#include "ScalarEntryWidget.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "System/BrightEyeStyle.h"
#include "Widgets/Images/SImage.h"


void SScalarEntryWidget::Construct(const FArguments& InArgs)
{
    // Store the delegate for value change
    OnValueChanged = InArgs._OnValueChanged;

    ChildSlot
    [
        SNew(SBorder)
        .BorderBackgroundColor(FLinearColor(0, 0, 0, 0.6))
        .BorderImage(EditorStyle::GetBrush("Brushes.Panel"))
        .Padding(5)
        .Clipping(EWidgetClipping::OnDemand)  
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SBox)
                .WidthOverride(70)
                [
                    SAssignNew(Title, STextBlock)
                    .Text(InArgs._Title)
                    .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                ]
            ]

            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            .Padding(FMargin(5.0f, 1.0f, 1.0f, 1.0f))
            [
                SNew(SOverlay)

                + SOverlay::Slot()
                [
                    SAssignNew(ProgressBar, SProgressBar)
                    .Visibility(EVisibility::HitTestInvisible)
                    .Style(&FBrightEyeStyle::GetCreatedToolSlateStyleSet()->GetWidgetStyle<FProgressBarStyle>("BrightEye.ProgressBar"))
                    .Percent(InArgs._InitialProgress) 
                ]

                + SOverlay::Slot()
                [
                    SAssignNew(Slider, SSlider)
                    .Value(InArgs._InitialProgress) 
                    .OnValueChanged(this, &SScalarEntryWidget::OnSliderValueChanged)
                    .Style(&FBrightEyeStyle::GetCreatedToolSlateStyleSet()->GetWidgetStyle<FSliderStyle>("BrightEye.Slider"))
                ]

                + SOverlay::Slot()
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                [
                    SAssignNew(ValueText, STextBlock)
                    .Visibility(EVisibility::HitTestInvisible)
                    .Text(FText::AsNumber(InArgs._InitialProgress * 100.0f)) 
                ]
            ]
        ]
    ];
}

void SScalarEntryWidget::OnSliderValueChanged(float NewValue) const
{
    ProgressBar->SetPercent(NewValue);
    ValueText->SetText(FText::FromString(FString::Printf(TEXT("%.2f"), NewValue * 100.0f))); 

    if (OnValueChanged.IsBound())
    {
        OnValueChanged.Execute(NewValue);
    }
}

FOnScalarParamChanged& SScalarEntryWidget::GetParamChangedSignature()
{
    return OnValueChanged;
}

void SScalarEntryWidget::UpdateProgressManually(const float& InValue) const
{
    Slider->SetValue(InValue);
    OnSliderValueChanged(Slider->GetValue());
}

