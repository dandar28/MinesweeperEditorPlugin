// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperEditorPlugin.h"
#include "MinesweeperEditorPluginStyle.h"
#include "MinesweeperEditorPluginCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

#include "Minesweeper/FMinesweeperGameSession.h"
#include "Minesweeper/FMinesweeperGameSettings.h"

#include "SMinesweeperGameBoard.h"

static const FName MinesweeperEditorPluginTabName("MinesweeperEditorPlugin");

#define LOCTEXT_NAMESPACE "FMinesweeperEditorPluginModule"

void FMinesweeperEditorPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FMinesweeperEditorPluginStyle::Initialize();
	FMinesweeperEditorPluginStyle::ReloadTextures();

	FMinesweeperEditorPluginCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FMinesweeperEditorPluginCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FMinesweeperEditorPluginModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMinesweeperEditorPluginModule::RegisterMenus));
}

void FMinesweeperEditorPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FMinesweeperEditorPluginStyle::Shutdown();

	FMinesweeperEditorPluginCommands::Unregister();
}

void FMinesweeperEditorPluginModule::PluginButtonClicked()
{
	// Prepare the game settings
	// \warning : Magic numbers here, to be removed.
	FMinesweeperGameSettings GameSettings;
	GameSettings.MatrixBoardSize.X = 9;
	GameSettings.MatrixBoardSize.Y = 9;
	GameSettings.NumberOfMines = 10;

	// Create the game session, start it up, prepare the game with the settings and start the game.
	const auto GameSession = MakeShared<FMinesweeperGameSession>();
	GameSession->Startup();
	GameSession->PrepareAndStartGame(GameSettings);

	// Create the target window with the Minesweeper Game Board for the started Game Session.
	TSharedRef<SWindow> MinesweeperGameWindow = SNew(SWindow)
		.Title(FText::FromString(TEXT("Minesweeper Game Window")))
		.ClientSize(FVector2D(800, 800))
		.SupportsMaximize(true)
		.SupportsMinimize(true)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SMinesweeperGameBoard)
				.GameSession(GameSession)
			]
		];

	// When the window is closed, shut the game session down as well.
	MinesweeperGameWindow->GetOnWindowClosedEvent().AddLambda([GameSession](const TSharedRef<SWindow>& InClosedWindow) {
		GameSession->Shutdown();
	});

	// Spawn the window and show it.
	FSlateApplication::Get().AddWindow(MinesweeperGameWindow, true);
}

void FMinesweeperEditorPluginModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FMinesweeperEditorPluginCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FMinesweeperEditorPluginCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMinesweeperEditorPluginModule, MinesweeperEditorPlugin)