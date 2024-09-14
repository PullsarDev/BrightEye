// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev

#pragma once

#include "CoreMinimal.h"

class UBESettings;
class UBEControlPanel;
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
	void Initialize();
	void Shutdown();
	
	void SetupDelegates();
	void RemoveDelegates() const;
	
private:
	void OnMapChanged(UWorld* World, EMapChangeType MapChangeType);
	auto OnActorSelectionChanged(const TArray<UObject*>& InActors, bool bIsSelected) const -> void;
	
	TWeakPtr<SLevelViewport> ActiveViewport;
	TWeakObjectPtr<AActor> ActiveActorLock;
	FTSTicker::FDelegateHandle TickHandle;
	bool OnTick(float DeltaTime);

	void OnActiveViewportChanged(TSharedPtr<IAssetViewport> OldViewport, TSharedPtr<IAssetViewport> NewViewport);
	
	void InitCommands();
	void OnToggleLight();
	void OnToggleControlPanel();
	
	void CreateBrightLight();
	void DestroyBrightLight();
	void UpdateLightTransformWithViewport(const float& InDeltaTime);
	
	UUserWidget* GetControlPanel();

	void InitializePanel();
	void RefreshColorPicker();
	void BindPanelDelegates();
	void UnbindPanelDelegates() const;
	void InitializePanelParams() const;
	
	void CreateAndShowControlPanel();
	void RemoveControlPanel() const;
	void DestroyControlPanel();
	
	FRotator BrightEyeRotation;
	void ResetBrightEyeRotation();
	
#pragma region InputPreProcessor
    
	void ActivateInputProcessor();
	void DeactivateInputProcessor();
    
	TSharedPtr<class FBEInputPreProcessor> InputProcessor;
	
protected:
	bool HandleKeySelected(const FKeyEvent& InKey);
	bool HandleKeyReleased(const FKeyEvent& InKey);
private:
	bool bIsPressed = true;

#pragma endregion InputPreProcessor
    
	TSharedPtr<class FUICommandList> CameraLevelCommands;

	TObjectPtr<AActor> BrightEyeActor;
	TObjectPtr<USpotLightComponent> BrightEyeComponent;
	TObjectPtr<UBEControlPanel> ControlPanelWidget;

	void OnScalarParamChanged(const float& InNewParam, EBEScalarParamType InParamType);
	void OnColorParamChanged(const FLinearColor& InNewColor);
	void OnSmoothRotationToggled();
	
	void OnBrightEyeSettingsChangedOnEditorSettings(const FPropertyChangedEvent& InPropertyChangedEvent);
	void OnResetBrightEyeSettings() const;

	void UpdateBrightEyeSpecs() const;
	void UpdateBrightness() const;
	void UpdateRadius() const;
	void UpdateDistance() const;
	void UpdateColor() const;


	bool bLightSettingsModified = false;
	float TimeSinceLastModification = 0.0f;

	void ResetLightModificationState();

	void ForceViewportRedraw() const;
};

class FBrightEyeManager
{
public:
	static void Initialize();
	static void Shutdown();
	static TSharedPtr<FBrightEyeManagerImp> BrightEyeManagerImp;
};

