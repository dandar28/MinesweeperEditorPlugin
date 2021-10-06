// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MinesweeperGame : ModuleRules
{
	public MinesweeperGame(ReadOnlyTargetRules Target) : base(Target)
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
