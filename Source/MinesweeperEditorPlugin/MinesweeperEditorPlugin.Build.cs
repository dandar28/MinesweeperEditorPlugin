// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MinesweeperEditorPlugin : ModuleRules
{
	public MinesweeperEditorPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"MinesweeperGame"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"UnrealEd",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"EditorStyle"
			}
			);
	}
}
