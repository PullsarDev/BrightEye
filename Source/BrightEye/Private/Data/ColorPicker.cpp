// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#include "ColorPicker.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Widgets/Layout/SBorder.h"

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
    if(!FSlateApplication::IsInitialized()){ return; }

    FColorPickerArgs PickerArgs;
    PickerArgs.bUseAlpha = true;
    PickerArgs.bOnlyRefreshOnMouseUp = false;
    PickerArgs.bOnlyRefreshOnOk = false;
#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 2)
    PickerArgs.InitialColor = OnGetColorForColorBlock();
#else
    PickerArgs.InitialColorOverride = OnGetColorForColorBlock();
#endif
    PickerArgs.sRGBOverride = true;
    PickerArgs.bUseAlpha = false;
    PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateUObject(this, &UBEColorPicker::ColorPicked);
    PickerArgs.OnColorPickerCancelled = FOnColorPickerCancelled::CreateUObject(this, &UBEColorPicker::OnColorPickerCancelled);
    PickerArgs.OnColorPickerWindowClosed = FOnWindowClosed::CreateUObject(this, &UBEColorPicker::OnColorPickerWindowClosed);
    PickerArgs.OnInteractivePickBegin = FSimpleDelegate::CreateUObject(this, &UBEColorPicker::OnColorPickerInteractiveBegin);
    PickerArgs.bOpenAsMenu = false;
    
    OpenColorPicker(PickerArgs);
}


void UBEColorPicker::ColorPicked(FLinearColor NewColor)
{
    SelectedColor = NewColor;
    
    if(OnColorSelected.IsBound())
    {
        OnColorSelected.Execute(NewColor);
    }
}
