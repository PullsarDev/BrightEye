// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScalarEntry.generated.h"


DECLARE_DELEGATE_OneParam(FOnScalarParamChanged,const float& /* Value */);

/**
 * Represents a UI element for scalar parameter entries in the Bright Eye tool.
 * This widget allows users to adjust scalar values such as brightness, radius, and distance, 
 * with precision control over the parameter changes.
 */

UCLASS()
class BRIGHTEYE_API UBEScalarEntry : public UUserWidget
{
	GENERATED_BODY()

protected:
	FOnScalarParamChanged OnScalarParamChanged;

	UFUNCTION(BlueprintCallable, Category = "Bright Eye UI")
	void CallOnScalarParamChanged(const float& InNewParam);

	UFUNCTION(BlueprintCallable,BlueprintPure, Category = "Bright Eye UI")
	FText GetRidOffDecimalPrecision(const float& InNewParam);
	
public:
	FOnScalarParamChanged& GetOnParamChangedDelegate();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Bright Eye UI")
	void InitializeScalarValue(const float& InInitialValue);
};
