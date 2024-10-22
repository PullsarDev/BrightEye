// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#include "GeometryUtils.h"
#include "LevelEditorViewport.h"

constexpr float MaxTraceDistance = 50000.0f;
constexpr float DefaultForwardDistance = 2000.0f;
constexpr float AlternativeTraceDistance = 3000.0f;

FRotator FGeometryUtils::AdjustLightRotationFromTrace(const FVector& InViewLocation,const FRotator& InViewRotation, const FVector& InLightLocation)
{
    UWorld* World  = GEditor->GetEditorWorldContext().World();
    if(!IsValid(World)){ return InViewRotation; }
    
    FVector TraceEnd = InViewLocation + (InViewRotation.Vector() * MaxTraceDistance);

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.bReturnPhysicalMaterial = false;

    bool bHit = World->LineTraceSingleByChannel(HitResult, InViewLocation, TraceEnd, ECC_Visibility, CollisionParams);

    if (bHit && HitResult.bBlockingHit)
    {
        return (HitResult.ImpactPoint - InLightLocation).Rotation();
    }

    return (InLightLocation + InViewRotation.Vector() * DefaultForwardDistance - InLightLocation).Rotation();
}


bool FGeometryUtils::GetHitLocationFromCameraAndMouse(FVector& OutHitLocation)
{
    UWorld* World = GEditor->GetEditorWorldContext().World();
    if (!World)
    {
        return false;
    }

    FLevelEditorViewportClient* ViewportClient = GCurrentLevelEditingViewportClient;
    if (!ViewportClient || !ViewportClient->Viewport)
    {
        return false;
    }

    FVector CameraLocation = ViewportClient->GetViewLocation();
    FVector CameraBackwardLocation = CameraLocation;

    FVector MouseWorldDirection = ViewportClient->GetCursorWorldLocationFromMousePos().GetDirection();
    FVector Direction = MouseWorldDirection;

    FHitResult HitResult;
    FVector TraceEnd = CameraBackwardLocation + (Direction * MaxTraceDistance);
    FCollisionQueryParams CollisionParams;
    CollisionParams.bReturnPhysicalMaterial = false;

    bool bHit = World->LineTraceSingleByChannel(HitResult, CameraBackwardLocation, TraceEnd, ECC_Visibility, CollisionParams);

    if (bHit && HitResult.bBlockingHit)
    {
        OutHitLocation = HitResult.Location;
        return true;
    }

    OutHitLocation = CameraBackwardLocation + (Direction * AlternativeTraceDistance);
    return false;
}
