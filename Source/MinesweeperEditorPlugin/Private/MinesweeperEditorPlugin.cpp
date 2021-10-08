// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperEditorPlugin.h"
#include "MinesweeperEditorPluginStyle.h"
#include "MinesweeperEditorPluginCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

#include "MinesweeperGameSession.h"
#include "MinesweeperGameBoard.h"

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
	const FIntPoint MatrixSize = FIntPoint(9, 9);

	const auto GameSession = MakeShared<FMinesweeperGameSession>();
	GameSession->Startup();
	GameSession->GetGameDataState()->RebuildMatrix(MatrixSize.X, MatrixSize.Y);

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

	MinesweeperGameWindow->GetOnWindowClosedEvent().AddLambda([GameSession]() {
		GameSession->Shutdown();
	});

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