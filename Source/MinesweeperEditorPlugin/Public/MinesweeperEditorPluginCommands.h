// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "MinesweeperEditorPluginStyle.h"

class FMinesweeperEditorPluginCommands : public TCommands<FMinesweeperEditorPluginCommands>
{
public:

	FMinesweeperEditorPluginCommands()
		: TCommands<FMinesweeperEditorPluginCommands>(TEXT("MinesweeperEditorPlugin"), NSLOCTEXT("Contexts", "MinesweeperEditorPlugin", "MinesweeperEditorPlugin Plugin"), NAME_None, FMinesweeperEditorPluginStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
