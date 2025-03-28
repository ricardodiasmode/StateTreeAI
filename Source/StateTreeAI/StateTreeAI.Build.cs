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
	        "NavigationAS",
	        "AIModule",
	        "StateTreeModule",
	        "GameplayTags",
	        "GameplayTasks",
	        "GameplayAbilities",
	        "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput", "GameplayStateTreeModule" });
    }
}
