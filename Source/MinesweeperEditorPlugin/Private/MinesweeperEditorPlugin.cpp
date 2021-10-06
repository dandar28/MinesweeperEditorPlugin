// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperEditorPlugin.h"
#include "MinesweeperEditorPluginStyle.h"
#include "MinesweeperEditorPluginCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

#include "MinesweeperGameSession.h"

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
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
							LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
							FText::FromString(TEXT("FMinesweeperEditorPluginModule::PluginButtonClicked()")),
							FText::FromString(TEXT("MinesweeperEditorPlugin.cpp"))
					   );
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
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