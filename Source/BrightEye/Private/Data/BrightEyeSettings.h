// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#pragma once

#include "CoreMinimal.h"
#include "BrightEyeSettings.generated.h"

DECLARE_DELEGATE_OneParam(FOnBrightLightSettingsChanged,const FPropertyChangedEvent&)
DECLARE_DELEGATE(FOnResetBrightEyeSettings)


/**
 * Stores configuration settings for the Bright Eye tool, including brightness, radius, distance, and color parameters.
 * This class allows for editing and saving tool-specific settings, with options for smoothing camera rotation and handling light visibility through keypresses.
 */
UCLASS(config = BrightEyeConfig)
class BRIGHTEYE_API UBESettings : public UObject
{
	GENERATED_BODY()

public:
	static UBESettings* GetInstance();

private:
	static UBESettings* SingletonInstance;

public:
	UPROPERTY(EditAnywhere,config,  Category = "Bright Eye",meta=(UIMin = 0.01, UIMax = 1,ClampMin = 0.01, ClampMax = 1,ToolTip = "Adjust the brightness of the Bright Eye."))
	float Brightness = 0.4f;

	UPROPERTY(EditAnywhere, config, Category = "Bright Eye",meta=(UIMin = 0.01, UIMax = 1,ClampMin = 0.01, ClampMax = 1,ToolTip = "Set the radius of the Bright Eye"))
	float Radius = 0.3f;
	
	UPROPERTY(EditAnywhere, config, Category = "Bright Eye",meta=(UIMin = 0.01, UIMax = 1,ClampMin = 0.01, ClampMax = 1,ToolTip = "Define the distance the Bright Eye's light reaches."))
	float Distance = 0.4f;

	UPROPERTY(EditAnywhere, config, Category = "Bright Eye",meta=(UIMin = 1,ToolTip = "Set the maximum brightness level for the Bright Eye."))
	float MaxBrightness = 500000;

	UPROPERTY(EditAnywhere, config, Category = "Bright Eye",meta=(UIMin = 2000, UIMax = 50000,ClampMin = 2000, ClampMax = 50000,ToolTip = "Determine the maximum distance the Bright Eye can illuminate."))
	float MaxDistance = 50000;

	UPROPERTY(EditAnywhere, config, Category = "Bright Eye",meta=(ToolTip = "Choose the color of the Bright Eye's light."))
	FLinearColor Color = FLinearColor::White;
	
	UPROPERTY(EditAnywhere, config, Category = "Bright Eye",meta=(ToolTip = "Toggle Bright Eye on/off with a key press, or hold to keep it on."))
	bool bActivateLightOnPress = false;

	UPROPERTY(EditAnywhere, config, Category = "Bright Eye",meta=(ToolTip = "Hold to keep the panel visible, or to<ggle on/off with a key press."))
	bool bHidePanelWhenIdle = false;
	
	UPROPERTY(EditAnywhere, config, Category = "Bright Eye", meta = (UIMin = 1, ToolTip = "Smooth the light rotation by adding a slight delay."))
	bool bSmoothLightRotation = false;

	UPROPERTY(EditAnywhere, config, Category = "Bright Eye", meta = (UIMin = 0.0, UIMax = 1.0, ClampMin = 0.0, ClampMax = 1.0, EditCondition="bSmoothLightRotation", ToolTip = "Set the delay factor for smoothing the camera rotation. Higher values result in more delay."))
	float RotationDelayFactor = 0.4f;

	UPROPERTY(EditAnywhere, config, Category = "Bright Eye", meta = (UIMin = -100.0f, UIMax = 100.0f, ClampMin = -100.0f, ClampMax = 100.0f, ToolTip = "Adjust the light offset relative to the camera's view, allowing fine-tuning of its position in the scene."))
	FVector2D LightViewOffset = FVector2D(); 

	UPROPERTY(EditAnywhere, config, Category = "Bright Eye", meta = (ToolTip = "Select a light profile (IES texture) to adjust the characteristics of the light's shape and distribution."))
	TSoftObjectPtr<UTextureLightProfile> LightProfile = nullptr;

	/* Resets the Bright Eye tool settings to their default values. */
	UFUNCTION(CallInEditor,Category = "Bright Eye")
	void ResetToolSettings();

	/* Opens the Bright Eye documentation in a web browser. */
	UFUNCTION(CallInEditor,Category = "Bright Eye")
	void OpenDocumentation() const;
	
	void SaveToolConfig();
	void LoadToolConfig();

	FOnBrightLightSettingsChanged OnBrightLightSettingsChanged;
	FOnResetBrightEyeSettings OnResetBrightEyeSettings;
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
};
