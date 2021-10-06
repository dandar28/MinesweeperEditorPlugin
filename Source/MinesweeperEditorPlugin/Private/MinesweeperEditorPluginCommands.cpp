// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperEditorPluginCommands.h"

#define LOCTEXT_NAMESPACE "FMinesweeperEditorPluginModule"

void FMinesweeperEditorPluginCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "MinesweeperEditorPlugin", "Execute MinesweeperEditorPlugin action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
