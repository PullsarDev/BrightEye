// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#pragma once

#include "CoreMinimal.h"

/**
 * FGeometryUtils provides utility functions for geometry calculations and editor viewport interactions.
 */

class BRIGHTEYE_API FGeometryUtils
{
public:

 static FRotator AdjustLightRotationFromTrace(const FVector& InViewLocation,const FRotator& InViewRotation, const FVector& InLightLocation);
 static bool GetHitLocationFromCameraAndMouse(FVector& OutHitLocation);
};
