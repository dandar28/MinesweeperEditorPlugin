// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MinesweeperGameLogics : ModuleRules
{
	public MinesweeperGameLogics(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
				
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Engine"
			}
			);
	}
}
