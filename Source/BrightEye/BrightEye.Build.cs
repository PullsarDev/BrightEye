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
		
		if (Target.Version.MajorVersion == 5 && Target.Version.MinorVersion <= 0)
		{
			PublicDependencyModuleNames.Add("EditorStyle");
		}
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UnrealEd",
				"EditorFramework",
				"LevelEditor",
				"Projects",
				"AppFramework",
				"UMG"
			}
		);
	}
}
