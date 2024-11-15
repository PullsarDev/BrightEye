// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#include "BrightEyePanel.h"
#include "CursorTrackerWidget.h"
#include "ScalarEntryWidget.h"
#include "Data/BrightEyeSettings.h"
#include "Data/ColorPicker.h"
#include "Data/PanelFadeOutManager.h"
#include "System/BrightEyeStyle.h"
#include "Viewports/InViewportUIDragOperation.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBox.h"

constexpr float DefaultFadeOutAmount = 0.5f;    
constexpr float DefaultFadeOutDuration = 0.4f; 
constexpr float DefaultFadeInDuration = 0.12f; 
constexpr float DefaultStartDelay = 0.8f;

void SBrightEyePanel::Construct(const FArguments& InArgs)
{
	Owner = InArgs._Owner;
	OnPanelDragStartedSignature = InArgs._OnPanelDragStarted;
	OnPanelDragFinishedSignature = InArgs._OnPanelDragFinished;
	OnBrightnessChangedSignature = InArgs._OnBrightnessChanged;
	OnRadiusChangedSignature = InArgs._OnRadiusChanged;
	OnDistanceChangedSignature = InArgs._OnDistanceChanged;
	OnSmoothRotationStateChanged = InArgs._OnSmoothRotationStateChanged;

	const FSlateBrush* SRSlateBrush = GetSmoothRotationButtonImage();

	UBESettings* BESettings = UBESettings::GetInstance();
	if(!IsValid(BESettings)){return;}
	
	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FBrightEyeStyle::GetCreatedToolSlateStyleSet()->GetBrush("BrightEye.PanelWithOutline"))
		.Padding(5)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(FMargin(2.0f))
			.HAlign(HAlign_Fill)
			.AutoHeight()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(30.0f)
					.HeightOverride(30.0f)
					[
						SNew(SButton)
						.ButtonStyle(FBrightEyeStyle::GetCreatedToolSlateStyleSet(),TEXT("BrightEye.BrightEyeButtonStyle"))
						.OnClicked(this, &SBrightEyePanel::OnSmoothRotationButtonClicked) 
						[
							SAssignNew(SmoothRotationImage,SImage)
							.Image(SRSlateBrush)
							.ColorAndOpacity(FColor::FromHex("#cfcfcf"))
						]
					]
				]

				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				.FillWidth(1)
				[
					SNew(SBorder)
					.Padding(0)
					.BorderImage(EditorStyle::GetBrush("NoBorder"))
					
					.OnMouseButtonDown_Lambda([this](const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
					{
						if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
						{
							return FReply::Handled().DetectDrag(SharedThis(this), EKeys::LeftMouseButton);
						}
						return FReply::Unhandled();
					})
					.OnMouseDoubleClick_Lambda([this](const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
					{
						if(!ScalarParamBox.IsValid() || !LightCoordOverlay.IsValid()){return FReply::Handled();}

						bool bIsInScalarMode = ScalarParamBox->GetVisibility() == EVisibility::Visible;

						ScalarParamBox->SetVisibility(bIsInScalarMode ? EVisibility::Hidden : EVisibility::Visible);
						LightCoordOverlay->SetVisibility(bIsInScalarMode ? EVisibility::Visible : EVisibility::Hidden);

						return FReply::Handled();
					})
					[
						SNew(SBox)
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Center)
						.Padding(FMargin(0,1,0,0))
						.MinDesiredWidth(160)
						[
							SNew(STextBlock)
							.ShadowOffset(FVector2D::UnitVector)
							.Font(FCoreStyle::GetDefaultFontStyle("Bold", 15))
							.Text(FText::FromString("Bright Eye Panel"))
							.Justification(ETextJustify::Center)
						]
					]
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(30.0f)
					.HeightOverride(30.0f)
					[
						SNew(SButton)
						.OnClicked(this, &SBrightEyePanel::OnColorButtonClicked)
						.ButtonStyle(FBrightEyeStyle::GetCreatedToolSlateStyleSet(),TEXT("BrightEye.BrightEyeButtonStyle"))
						[
							SNew(SImage)
							.Image(FSlateIcon(FBrightEyeStyle::GetToolStyleName(),"BrightEye.Color").GetIcon())
							.ColorAndOpacity(FColor::FromHex("#cfcfcf"))
						]
					]
				]
			]
			
			+ SVerticalBox::Slot()
			.Padding(FMargin(1.0f))
			[
				SNew(SBox)
				.HeightOverride(90)
				.WidthOverride(222)
				[
					SNew(SOverlay)
					+SOverlay::Slot()
					[
						SAssignNew(ScalarParamBox,SVerticalBox)
						.Visibility(EVisibility::Visible)
						
						+SVerticalBox::Slot()
						.Padding(FMargin(1.0f))
						[
							SAssignNew(BrightnessEntry,SScalarEntryWidget)
							.Title(FText::FromString("Brightness"))
							.InitialProgress(BESettings->Brightness)
							.OnValueChanged(FOnScalarParamChanged::CreateLambda([this](const float& Value)
							{
								if(OnBrightnessChangedSignature.IsBound())
								{
									OnBrightnessChangedSignature.Execute(Value);
								}
							}))
						]
						+ SVerticalBox::Slot()
						.Padding(FMargin(1.0f))
						[
							SAssignNew(RadiusEntry,SScalarEntryWidget)
							.Title(FText::FromString("Radius"))
							.InitialProgress(BESettings->Radius)
							.OnValueChanged(FOnScalarParamChanged::CreateLambda([this](const float& Value)
							{
								if(OnRadiusChangedSignature.IsBound())
								{
									OnRadiusChangedSignature.Execute(Value);
								}
							}))
						]
						+ SVerticalBox::Slot()
						.Padding(FMargin(1.0f))
						[
							SAssignNew(DistanceEntry,SScalarEntryWidget)
							.Title(FText::FromString("Distance"))
							.InitialProgress(BESettings->Distance)
							.OnValueChanged(FOnScalarParamChanged::CreateLambda([this](const float& Value)
							{
								if(OnDistanceChangedSignature.IsBound())
								{
									OnDistanceChangedSignature.Execute(Value);
								}
							}))
						]
					]

					+SOverlay::Slot()
					[
						SAssignNew(LightCoordOverlay,SOverlay)
						.Visibility(EVisibility::Hidden)
						+SOverlay::Slot()
						[
							SNew(SBorder)
							.BorderImage(FBrightEyeStyle::GetCreatedToolSlateStyleSet()->GetBrush("BrightEye.GridPanelWithOutline"))
							.Padding(0.5f)
							[
								SNew(SBox)
								.Padding(1.5f)
								[
									SNew(SImage)
									.Image(FSlateIcon(FBrightEyeStyle::GetToolStyleName(),"BrightEye.Grid").GetIcon())
									.RenderOpacity(0.9f)
								]
							]
						]

						+SOverlay::Slot()
						[
							SNew(SBox)
							.Padding(4)
							[
								SAssignNew(CursorTrackerWidget, SCursorTrackerWidget)
								.InitialCoords(BESettings->LightViewOffset / 100.0f)
								.OnCoordChanged(InArgs._OnCoordsChanged)
							]
						]
					]
				]
			]
		]
	];

	CreateFadeOutManager();
}

FReply SBrightEyePanel::OnSmoothRotationButtonClicked() const
{
	if(OnSmoothRotationStateChanged.IsBound())
	{
		OnSmoothRotationStateChanged.Execute();
	}

	RefreshCameraRotationState();
	
	return FReply::Handled();
}



FReply SBrightEyePanel::OnColorButtonClicked() const
{
	if(IsValid(ColorPicker))
	{
		ColorPicker->OpenColorPickerInternal();
	}
	return FReply::Handled();
}


bool SBrightEyePanel::InitializeTheColorPicker()
{
	if (!IsValid(ColorPicker) && Owner.IsValid())
	{
		ColorPicker = NewObject<UBEColorPicker>(Owner.Get());
		return true;
	}
	return false;
}

UBEColorPicker* SBrightEyePanel::GetColorPicker()
{
	if(!IsValid(ColorPicker))
	{
		InitializeTheColorPicker();
	}
	return ColorPicker;
}

void SBrightEyePanel::RefreshPanel() const
{
	UBESettings* BESettings = UBESettings::GetInstance();
	if(!IsValid(BESettings)){return;}
	
	if(BrightnessEntry.IsValid()){BrightnessEntry->UpdateProgressManually(BESettings->Brightness);}
	if(RadiusEntry.IsValid()){RadiusEntry->UpdateProgressManually(BESettings->Radius);}
	if(DistanceEntry.IsValid()){DistanceEntry->UpdateProgressManually(BESettings->Distance);}
	if(IsValid(ColorPicker)){ColorPicker->InitializeColor(BESettings->Color);}
	RefreshCameraRotationState();
}

void SBrightEyePanel::RefreshBrightness() const
{
	UBESettings* BESettings = UBESettings::GetInstance();
	if(!IsValid(BESettings)){return;}
	if(BrightnessEntry.IsValid()){BrightnessEntry->UpdateProgressManually(BESettings->Brightness);}
}

void SBrightEyePanel::RefreshRadius() const
{
	UBESettings* BESettings = UBESettings::GetInstance();
	if(!IsValid(BESettings)){return;}
	if(RadiusEntry.IsValid()){RadiusEntry->UpdateProgressManually(BESettings->Radius);}
}

void SBrightEyePanel::RefreshDistance() const
{
	UBESettings* BESettings = UBESettings::GetInstance();
	if(!IsValid(BESettings)){return;}
	if(DistanceEntry.IsValid()){DistanceEntry->UpdateProgressManually(BESettings->Distance);}
}

void SBrightEyePanel::RefreshCameraRotationState() const
{
	if(SmoothRotationImage.IsValid()){SmoothRotationImage->SetImage(GetSmoothRotationButtonImage());}
}

void SBrightEyePanel::RefreshColor() const
{
	UBESettings* BESettings = UBESettings::GetInstance();
	if(IsValid(ColorPicker)){ColorPicker->InitializeColor(BESettings->Color);}
}

void SBrightEyePanel::ChangeHidePanelWhenIdle(bool InHidePanelWhenIdle)
{
	bHidePanelWhenIdle = InHidePanelWhenIdle;

	if (FadeOutManager)
	{
		if(bHidePanelWhenIdle)
		{
			FadeOutManager->StartFadeOut(SharedThis(this), DefaultFadeOutAmount, DefaultFadeOutDuration, DefaultStartDelay);  
		}
		else
		{
			FadeOutManager->StopAndStartFadeIn(SharedThis(this), DefaultFadeInDuration);  
		}
	}
}

void SBrightEyePanel::UpdateViewCoords() const
{
	if(!CursorTrackerWidget.IsValid() || !UBESettings::GetInstance()){return;}

	CursorTrackerWidget->UpdateViewCoords(UBESettings::GetInstance()->LightViewOffset / 100.0f);
}


const FSlateBrush* SBrightEyePanel::GetSmoothRotationButtonImage()
{
	const FSlateBrush* DelayBrush = nullptr;
	
	if(UBESettings::GetInstance())
	{
		DelayBrush = UBESettings::GetInstance()->bSmoothLightRotation ?
			FSlateIcon(FBrightEyeStyle::GetToolStyleName(),"BrightEye.Motion").GetIcon() :
			FSlateIcon(FBrightEyeStyle::GetToolStyleName(),"BrightEye.Still").GetIcon() ;
	}
	return DelayBrush;
}


FReply SBrightEyePanel::OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	const FVector2D TabGrabScreenSpaceOffset = MouseEvent.GetScreenSpacePosition() - MyGeometry.GetAbsolutePosition();

	FOnInViewportUIDropped OnUIDropped = FOnInViewportUIDropped::CreateSP(this, &SBrightEyePanel::FinishDraggingWidget);
	TSharedRef<FInViewportUIDragOperation> DragDropOperation =
		FInViewportUIDragOperation::New(
			SharedThis(this),
			TabGrabScreenSpaceOffset,
			GetDesiredSize(),
			OnUIDropped
		);

	if (OnPanelDragStartedSignature.IsBound())
	{
		OnPanelDragStartedSignature.Execute();
	}
	
	return FReply::Handled().BeginDragDrop(DragDropOperation);
}

void SBrightEyePanel::FinishDraggingWidget(const FVector2D InLocation) const
{
	if (OnPanelDragFinishedSignature.IsBound())
	{
		OnPanelDragFinishedSignature.Execute(InLocation);
	}
}

void SBrightEyePanel::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	SCompoundWidget::OnMouseEnter(MyGeometry, MouseEvent);
	
	if (bHidePanelWhenIdle && FadeOutManager)
	{
		FadeOutManager->StopAndStartFadeIn(SharedThis(this), DefaultFadeInDuration);  
	}
}

void SBrightEyePanel::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	SCompoundWidget::OnMouseLeave(MouseEvent);
	
	if (bHidePanelWhenIdle && FadeOutManager)
	{
		FadeOutManager->StartFadeOut(SharedThis(this), DefaultFadeOutAmount, DefaultFadeOutDuration, DefaultStartDelay);  
	}
}

void SBrightEyePanel::CreateFadeOutManager()
{
	if (!FadeOutManager)
	{
		FadeOutManager = new FPanelFadeOutManager(); 
	}
}


SBrightEyePanel::~SBrightEyePanel()
{
	if (FadeOutManager)
	{
		delete FadeOutManager; 
		FadeOutManager = nullptr;
	}
}
