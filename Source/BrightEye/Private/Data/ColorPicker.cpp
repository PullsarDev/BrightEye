// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev

#include "ColorPicker.h"

#include "Blueprint/UserWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Widgets/Layout/SBorder.h"

void UBEColorPicker::InitializeParent(const TSharedRef<SWidget>& InParent)
{
    Parent = InParent.ToSharedPtr();
}

void UBEColorPicker::InitializeColor(const FLinearColor& InInitialValue)
{
    FormalColor = InInitialValue;
    SelectedColor = InInitialValue;
}

FOnColorChanged& UBEColorPicker::GetOnParamChangedDelegate()
{
    return OnColorSelected;
}

FLinearColor UBEColorPicker::OnGetColorForColorBlock() const
{
    return SelectedColor;
}

void UBEColorPicker::OnColorPickerCancelled(FLinearColor LinearColor)
{
    bCancelled = true;
    
}

void UBEColorPicker::OnColorPickerWindowClosed(const TSharedRef<SWindow>& Window)
{
    if(bCancelled)
    {
        SelectedColor = FormalColor;
    }
    else
    {
        FormalColor = SelectedColor;
    }
    
    if(OnColorSelected.IsBound())
    {
        OnColorSelected.Execute(SelectedColor);
    }
    
    bCancelled = false;
}

void UBEColorPicker::OnColorPickerInteractiveBegin()
{
    SelectedColor = FormalColor;
}

void UBEColorPicker::OnColorPickerInteractiveEnd()
{
    //No action required
}

FReply UBEColorPicker::OnMouseButtonDownColorBlock(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        OpenColorPickerInternal();
        return FReply::Handled();
    }
    return FReply::Unhandled();
}

void UBEColorPicker::OpenColorPickerInternal()
{
    auto Test = Cast<UUserWidget>(GetOuter());
    if(!IsValid(Test)){return;}
    auto Ref = Cast<UUserWidget>(GetOuter())->TakeWidget();
    
    FColorPickerArgs PickerArgs;
    PickerArgs.bUseAlpha = true;
    PickerArgs.bOnlyRefreshOnMouseUp = false;
    PickerArgs.bOnlyRefreshOnOk = false;
    PickerArgs.InitialColor = OnGetColorForColorBlock();
    PickerArgs.sRGBOverride = true;
    PickerArgs.bUseAlpha = false;
    PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateUObject(this, &UBEColorPicker::ColorPicked);
    PickerArgs.OnColorPickerCancelled = FOnColorPickerCancelled::CreateUObject(this, &UBEColorPicker::OnColorPickerCancelled);
    PickerArgs.OnColorPickerWindowClosed = FOnWindowClosed::CreateUObject(this, &UBEColorPicker::OnColorPickerWindowClosed);
    PickerArgs.OnInteractivePickBegin = FSimpleDelegate::CreateUObject(this, &UBEColorPicker::OnColorPickerInteractiveBegin);
    PickerArgs.OnInteractivePickEnd = FSimpleDelegate::CreateUObject(this, &UBEColorPicker::OnColorPickerInteractiveEnd);
    //PickerArgs.ParentWidget = ColorBlock;
    PickerArgs.OptionalOwningDetailsView = Ref;
    FWidgetPath ParentWidgetPath;
    if (FSlateApplication::Get().FindPathToWidget(Ref, ParentWidgetPath))
    {
        PickerArgs.bOpenAsMenu = FSlateApplication::Get().FindMenuInWidgetPath(ParentWidgetPath).IsValid();
    }
    
    OpenColorPicker(PickerArgs);  // Call the global OpenColorPicker function
}

void UBEColorPicker::ColorPicked(FLinearColor NewColor)
{
    SelectedColor = NewColor;
    
    if(OnColorSelected.IsBound())
    {
        OnColorSelected.Execute(NewColor);
    }
}
