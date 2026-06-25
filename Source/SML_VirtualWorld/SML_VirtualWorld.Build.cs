// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SML_VirtualWorld : ModuleRules
{
	public SML_VirtualWorld(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Slate", "SlateCore", "HTTP", "Json", "JsonUtilities" });
	}
}
