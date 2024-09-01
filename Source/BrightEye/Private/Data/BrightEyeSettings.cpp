// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#include "BrightEyeSettings.h"
#include "Interfaces/IPluginManager.h"

UBESettings* UBESettings::SingletonInstance = nullptr;

UBESettings* UBESettings::GetInstance()
{
	if (SingletonInstance == nullptr)
	{
		SingletonInstance = NewObject<UBESettings>();
		SingletonInstance->AddToRoot(); 
		SingletonInstance->LoadToolConfig(); 
	}
	return SingletonInstance;
}

void UBESettings::ResetToolSettings()
{
	Brightness = 0.4f;
	Radius = 0.3f;
	Distance = 0.4f;
	MaxBrightness = 500000;
	MaxDistance = 50000;
	Color = FLinearColor::White;
	bActivateLightOnPress = false;
	bHidePanelWhenIdle = false;
	bSmoothCameraRotation = false;
	RotationDelayFactor = 0.4f;
	
	if(OnResetBrightEyeSettings.IsBound())
	{
		OnResetBrightEyeSettings.Execute();
	}
}

void UBESettings::OpenDocumentation() const
{
	const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("BrightEye"));

	if (Plugin.IsValid()) {
		const FPluginDescriptor &Descriptor = Plugin->GetDescriptor();
		FPlatformProcess::LaunchURL(*Descriptor.DocsURL, nullptr, nullptr);
	}
}

void UBESettings::SaveToolConfig()
{
	const FString PluginConfigDir = IPluginManager::Get().FindPlugin(TEXT("BrightEye"))->GetBaseDir() / "Config";
	FString PluginConfigFile = FPaths::Combine(PluginConfigDir, TEXT("BrightEyeSettings.ini"));
	SaveConfig(CPF_Config, *FConfigCacheIni::NormalizeConfigIniPath(PluginConfigFile));
	
}

void UBESettings::LoadToolConfig()
{
	const FString PluginConfigDir = IPluginManager::Get().FindPlugin(TEXT("BrightEye"))->GetBaseDir() / "Config";
	FString PluginConfigFile = FConfigCacheIni::NormalizeConfigIniPath(FPaths::Combine(PluginConfigDir, TEXT("BrightEyeSettings.ini")));

	if (FPaths::FileExists(PluginConfigFile))
	{
		LoadConfig(GetClass(), *PluginConfigFile);
	}
	else
	{
		LoadConfig();
	}
}


void UBESettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	if(OnBrightLightSettingsChanged.IsBound())
	{
		OnBrightLightSettingsChanged.Execute(PropertyChangedEvent);
	}
}
