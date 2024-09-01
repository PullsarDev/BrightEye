// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev

#include "BrightEye.h"
#include "BrightEyeManager.h"
#include "ISettingsModule.h"
#include "Data/BrightEyeSettings.h"

#define LOCTEXT_NAMESPACE "FBrightEyeModule"

void FBrightEyeModule::StartupModule()
{
	RegisterToolSettings();
	
	FBrightEyeManager::Initialize();
}

void FBrightEyeModule::ShutdownModule()
{
	FBrightEyeManager::Shutdown();

	UnregisterToolSettings();
	
}


void FBrightEyeModule::RegisterToolSettings()
{
	UBESettings* Config = UBESettings::GetInstance();

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Editor", "Plugins", "Bright Eye",
			LOCTEXT("BrightEyeSettingsName", "Bright Eye"),
			LOCTEXT("BrightEyeSettingsDescription", "Configure the settings for BrightEye."),
			Config); 
	}
}

void FBrightEyeModule::UnregisterToolSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Editor", "Plugins", "Bright Eye");
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBrightEyeModule, BrightEye)