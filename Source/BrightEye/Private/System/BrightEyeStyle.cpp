// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#include "BrightEyeStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateTypes.h"

FName FBrightEyeStyle::ToolStyleName = FName("BrightEyeStyle");
TSharedPtr<FSlateStyleSet> FBrightEyeStyle::CreatedToolSlateStyleSet = nullptr;

void FBrightEyeStyle::InitializeToolStyle()
{	
	if(!CreatedToolSlateStyleSet.IsValid())
	{
		CreatedToolSlateStyleSet = CreateToolSlateStyleSet();
		FSlateStyleRegistry::RegisterSlateStyle(*CreatedToolSlateStyleSet);
	}
}

TSharedRef<FSlateStyleSet> FBrightEyeStyle::CreateToolSlateStyleSet()
{	
	TSharedRef<FSlateStyleSet> BrightEyeStyleSet = MakeShareable(new FSlateStyleSet(ToolStyleName));

	const FString IconDirectory = IPluginManager::Get().FindPlugin(TEXT("BrightEye"))->GetBaseDir() /"Resources";
	
	BrightEyeStyleSet->SetContentRoot(IconDirectory);

	const FVector2D Icon30x30 (30.f,30.f);
	const FVector2D Icon32x32 (30.f,30.f);
	
	BrightEyeStyleSet->Set("BrightEye.Motion", new FSlateImageBrush(IconDirectory / "Motion.png", Icon30x30)); 
	BrightEyeStyleSet->Set("BrightEye.Still", new FSlateImageBrush(IconDirectory / "Still.png", Icon30x30));
	BrightEyeStyleSet->Set("BrightEye.Color", new FSlateImageBrush(IconDirectory / "Color.png", Icon30x30));
	BrightEyeStyleSet->Set("BrightEye.Target", new FSlateImageBrush(IconDirectory / "Target.png", Icon32x32));
	BrightEyeStyleSet->Set("BrightEye.Grid", new FSlateImageBrush(IconDirectory / "Grid.png", FVector2D(217.0f,87.0f)));

	FSlateBrush* BorderBrush = new FSlateRoundedBoxBrush(
	FLinearColor(0, 0, 0, 0.5f), 
	4.0f,                     
	FLinearColor(FColor::FromHex("#080808")),          
	3.0f                
	);

	FSlateBrush* GridBorderBrush = new FSlateRoundedBoxBrush(
	FLinearColor(0, 0, 0, 0.5f), 
	4.0f,                     
	FLinearColor(FColor::FromHex("#080808")),          
	1.0f                
	);
	
	BrightEyeStyleSet->Set("BrightEye.PanelWithOutline", BorderBrush);
	BrightEyeStyleSet->Set("BrightEye.GridPanelWithOutline", GridBorderBrush);
	
	const FButtonStyle BrightEyeButtonStyle = FButtonStyle()
		.SetNormal(FSlateRoundedBoxBrush(FLinearColor(FColor::FromHex("#262626")), 6.0f, FLinearColor::Black, 1.0f))
		.SetHovered(FSlateRoundedBoxBrush(FLinearColor(FColor::FromHex("#262626")), 6.0f, FLinearColor::Black, 1.0f))
		.SetPressed(FSlateRoundedBoxBrush(FLinearColor(FColor::FromHex("#262626")), 6.0f, FLinearColor::Black, 1.0f))
		.SetNormalPadding(FMargin(2,2 ))
		.SetPressedPadding(FMargin(3, 3));
	BrightEyeStyleSet->Set("BrightEye.BrightEyeButtonStyle",BrightEyeButtonStyle);
	
	FSliderStyle SliderStyle = FSliderStyle()
		.SetNormalBarImage(FSlateColorBrush(FLinearColor(0, 0, 0, 0))) 
		.SetHoveredBarImage(FSlateColorBrush(FLinearColor(0, 0, 0, 0)))
		.SetNormalThumbImage(FSlateColorBrush(FLinearColor(0, 0, 0, 0)))  
		.SetHoveredThumbImage(FSlateColorBrush(FLinearColor(0, 0, 0, 0))) 
		.SetBarThickness(0.0f); 
	BrightEyeStyleSet->Set("BrightEye.Slider", SliderStyle);
	
	FProgressBarStyle ProgressBarStyle = FProgressBarStyle()
		.SetBackgroundImage(FSlateRoundedBoxBrush(FLinearColor(FColor::FromHex("#222222")), 2.0f, FLinearColor(FColor::FromHex("#222222")), 2.0f)) 
		.SetFillImage(FSlateRoundedBoxBrush(FLinearColor(FColor::FromHex("#505050")), 2.0f, FLinearColor(FColor::FromHex("#505050")), 1.0f))
		.SetMarqueeImage(FSlateRoundedBoxBrush(FLinearColor(FColor::FromHex("#007BFF")), 2.0f, FLinearColor(FColor::FromHex("#007BFF")), 1.0f));
	BrightEyeStyleSet->Set("BrightEye.ProgressBar", ProgressBarStyle);
	
	return BrightEyeStyleSet;
}

void FBrightEyeStyle::ShutDownStyle()
{
	if(CreatedToolSlateStyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*CreatedToolSlateStyleSet);
		CreatedToolSlateStyleSet.Reset();
	}
}
