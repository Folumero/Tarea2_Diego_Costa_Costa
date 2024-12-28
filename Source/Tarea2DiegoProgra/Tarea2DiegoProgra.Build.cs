// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Tarea2DiegoProgra : ModuleRules
{
	public Tarea2DiegoProgra(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
