// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev


#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FBrightEyeModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	static void RegisterToolSettings();
	static void UnregisterToolSettings();
};
