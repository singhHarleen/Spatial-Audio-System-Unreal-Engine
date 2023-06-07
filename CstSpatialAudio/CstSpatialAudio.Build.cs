// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CstSpatialAudio : ModuleRules
{
	public CstSpatialAudio(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
	}
}
