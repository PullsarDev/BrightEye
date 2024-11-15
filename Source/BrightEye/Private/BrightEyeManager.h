// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#pragma once

#include "CoreMinimal.h"
#include "UnrealEdMisc.h"

class SBrightEyePanel;
class UBESettings;
class SBEControlPanel;
class USpotLightComponent;
class SLevelViewport;

enum class EBEScalarParamType
{
	Brightness,
	Distance,
	Radius
};

/**
 * Manages BrightEye tool behavior, including light creation, control panel interactions, and input processing.
 */
class FBrightEyeManagerImp : public TSharedFromThis<FBrightEyeManagerImp>
{
public:
    // Initialization and Shutdown
    void Initialize();
    void Shutdown();

    // Delegate setup
    void SetupDelegates();
    void RemoveDelegates() const;

private:
    void HandleBeginPIE(bool bIsSimulating);
    
    // Tick function
    bool OnTick(float InDeltaTime);
    
    // Input handling
    void ActivateInputProcessor();
    void DeactivateInputProcessor();
    bool HandleKeySelected(const FKeyEvent& InKey);
    bool HandleKeyReleased(const FKeyEvent& InKey);
    
    // Light management
    void CreateBrightEyeLight();
    void DestroyBrightLight();
    void UpdateLightTransformWithViewport(const float& InDeltaTime);
    void UpdateBrightEyeSpecs() const;
    void UpdateBrightness() const;
    void UpdateRadius() const;
    void UpdateDistance() const;
    void UpdateColor() const;
    void UpdateLightProfile() const;
    void UpdateLightViewOffsetOnPanel() const;
    void ResetBrightEyeRotation();
    
    // Bright Eye Panel management
    void InitializePanel();
    void CreateBrightEyePanel();
    void DestroyBrightEyePanel();
    void TryRevealBrightEyePanel();
    void TryHideBrightEyePanel();
    void UpdateBrightEyePanelLocation(FVector2D InLocation);
    FMargin GetBrightEyePanelPadding() const;
    void ResetPanelLocation();
    void RefreshColorPicker();
    void InitializePanelParams() const;

    // Scalar and color parameter changes
    void OnScalarParamChanged(const float& InNewParam, EBEScalarParamType InParamType);
    void OnColorParamChanged(const FLinearColor& InNewColor);
    void OnSmoothRotationToggled();
    void OnBrightEyeSettingsChangedOnEditorSettings(const FPropertyChangedEvent& InPropertyChangedEvent);
    void OnResetBrightEyeSettings() const;
    
    // Panel dragging and drop checking
    void OnPanelDragStarted();
    void OnPanelDragFinished(const FVector2D& InDropLocation);
    void CheckForOutOfBoundDropping(const float& InDeltaTime);

    // Panel dragging and drop checking
    void OnCoordsChanged(const FVector2D& InNewCoords);

    
    // Viewport management
    void OnActiveViewportChanged(TSharedPtr<IAssetViewport> OldViewport, TSharedPtr<IAssetViewport> NewViewport);
    void OnMapChanged(UWorld* World, EMapChangeType MapChangeType);
    void OnActorSelectionChanged(const TArray<UObject*>& InActors, bool bIsSelected) const;

    // Light modification state
    void ResetLightModificationState();
    void ForceViewportRedraw() const;

    // Command initialization
    void InitCommands(); 
    void OnToggleLight();  
    void OnToggleBrightEyePanel();
    void InvalidateViewport() const;
    void SetLightVisibility(bool bVisible) const;
    void OnAimBrightEye();

    static bool IsInPie();

    // Light-related variables
    TObjectPtr<AActor> BrightEyeActor;
    TObjectPtr<USpotLightComponent> BrightEyeComponent;
    FRotator BrightEyeRotation = FRotator();
    bool bLightSettingsModified = false;
    float TimeSinceLastModification = 0.0f;

    // Panel-related variables
    TSharedPtr<SWidget> BrightEyePanelParent;
    TSharedPtr<class SBrightEyePanel> BrightEyePanel;
    FVector2D BEPanelLocation = FVector2D();
    float PanelDropTimer  = 0;
    bool bIsDragging = false;
    bool bIsAimLighting = false;
    bool bIsLightActiveBeforeAiming = false;
    bool bCheckingForDrop = false;
    bool bIsPanelOnWindow = false;

    // Input processing
    bool bIsAnyControlKeyPressed = true;
    TSharedPtr<class FBEInputPreProcessor> InputProcessor;
    TSharedPtr<class FUICommandList> CameraLevelCommands;
    
    // Viewport tracking
    TWeakPtr<SLevelViewport> ActiveViewport;
    FTSTicker::FDelegateHandle TickHandle;
};

class FBrightEyeManager
{
public:
    static void Initialize();
    static void Shutdown();
    static TSharedPtr<FBrightEyeManagerImp> BrightEyeManagerImp;
};


