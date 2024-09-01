// Copyright (c) 2024 PullsarDev - GitHub: https://github.com/PullsarDev

using UnrealBuildTool;

public class BrightEye : ModuleRules
{
	public BrightEye(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"InputCore",
			}
			);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UnrealEd",
				"LevelEditor",
				"Projects",
				"AppFramework",
				"UMG"
			}
			);
	}
}
