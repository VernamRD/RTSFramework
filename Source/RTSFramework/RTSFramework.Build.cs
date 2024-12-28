// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RTSFramework : ModuleRules
{
	public RTSFramework(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "GLT" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
