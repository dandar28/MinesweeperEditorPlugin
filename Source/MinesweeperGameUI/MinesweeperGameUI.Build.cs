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
				"InputCore",
				"Slate",
				"SlateCore",
				"EditorStyle",
                "Projects",
                "MinesweeperGameLogics"
            }
		);
	}
}
