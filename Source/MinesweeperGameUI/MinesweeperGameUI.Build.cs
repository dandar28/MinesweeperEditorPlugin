// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MinesweeperGameUI : ModuleRules
{
	public MinesweeperGameUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
				
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Engine",
				"Slate",
				"SlateCore",
				"EditorStyle",
				"MinesweeperGameLogics"
			}
			);
	}
}
