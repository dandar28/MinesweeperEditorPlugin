// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperGameUIStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr< FSlateStyleSet > FMinesweeperGameUIStyle::StyleInstance = NULL;

void FMinesweeperGameUIStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FMinesweeperGameUIStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FMinesweeperGameUIStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("MinesweeperGameUIStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

TSharedRef< FSlateStyleSet > FMinesweeperGameUIStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("MinesweeperGameUIStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("MinesweeperEditorPlugin")->GetBaseDir() / TEXT("Resources"));

	Style->Set(
		"MinesweeperGameUI.NumberOfMineStyle",
		FTextBlockStyle(FTextBlockStyle::GetDefault())
			.SetFont(TTF_FONT("kenyan-coffee.rg-bold", 40))
			.SetFontSize(40)
	);

	Style->Set(
		"MinesweeperGameUI.TimerDisplayStyle",
		FTextBlockStyle(FTextBlockStyle::GetDefault())
		.SetFont(TTF_FONT("digital-7 (mono)", 40))
		.SetFontSize(40)
	);	

	const FVector2D Icon40x40(40.0f, 40.0f);
	Style->Set("MinesweeperGameUI.Bomb", new IMAGE_BRUSH(TEXT("clanbomber"), Icon40x40));
	Style->Set("MinesweeperGameUI.Flag", new IMAGE_BRUSH(TEXT("red-flag"), Icon40x40));

	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FMinesweeperGameUIStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FMinesweeperGameUIStyle::Get()
{
	return *StyleInstance;
}
