// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StateTreeAI : ModuleRules
{
	public StateTreeAI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", 
	        "CoreAS",
	        "CoreUObject", 
	        "AIModule",
	        "StateTreeModule",
	        "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput", "GameplayStateTreeModule" });
    }
}
