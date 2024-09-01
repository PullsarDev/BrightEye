// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#include "ScalarEntry.h"


void UBEScalarEntry::CallOnScalarParamChanged(const float& InNewParam)
{
	if(OnScalarParamChanged.IsBound())
	{
		OnScalarParamChanged.Execute(InNewParam);
	}
}

FOnScalarParamChanged& UBEScalarEntry::GetOnParamChangedDelegate()
{
	return OnScalarParamChanged;
}

FText UBEScalarEntry::GetRidOffDecimalPrecision(const float& InNewParam)
{
	return FText::FromString(FString::Printf(TEXT("%.2f"), InNewParam));
}


