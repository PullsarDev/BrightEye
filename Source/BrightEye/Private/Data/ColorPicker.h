// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ColorPicker.generated.h"


DECLARE_DELEGATE_OneParam(FOnColorChanged,const FLinearColor& /* Value */);


/**
 * Provides a color picker utility for selecting and managing the color of the Bright Eye's light.
 * This class handles the color selection process, user interaction with the color block, 
 * and integration with the Bright Eye tool's UI.
 */

UCLASS()
class BRIGHTEYE_API UBEColorPicker : public UObject
{
	GENERATED_BODY()

	TSharedPtr<SWidget> Parent;
	
public:
	void InitializeParent(const TSharedRef<SWidget>& InParent);
	
	void InitializeColor(const FLinearColor& InInitialValue);

	FOnColorChanged& GetOnParamChangedDelegate();

	UFUNCTION(BlueprintCallable, Category = "Bright Eye UI")
	void OpenColorPickerInternal();

protected:
	// Function to handle color block interaction
	FReply OnMouseButtonDownColorBlock(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	// Helper function to get color for the color block
	FLinearColor OnGetColorForColorBlock() const;

	void OnColorPickerCancelled(FLinearColor LinearColor);
	void OnColorPickerWindowClosed(const TSharedRef<SWindow>& Window);
	void OnColorPickerInteractiveBegin();
	void OnColorPickerInteractiveEnd();
	void ColorPicked(FLinearColor NewColor);
private:
	TSharedPtr<class SColorBlock> ColorBlock;
	TSharedPtr<class SColorPicker> ColorPicker;

	UPROPERTY()
	FLinearColor FormalColor;

	UPROPERTY()
	FLinearColor SelectedColor;
	
	FOnColorChanged OnColorSelected;

	bool bCancelled = false;
};
