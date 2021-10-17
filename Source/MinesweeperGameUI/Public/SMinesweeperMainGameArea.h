#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

#include "SNumericSettingEntry.h"
#include "SMinesweeperCell.h"

#include "Minesweeper/FMinesweeperGameSession.h"

/**
 * \brief - Slate widget that renders the main game area of a minesweeper game session.
 */
class MINESWEEPERGAMEUI_API SMinesweeperMainGameArea
	: public SCompoundWidget {
public:

	SLATE_BEGIN_ARGS(SMinesweeperMainGameArea)
		: _GameSession(nullptr)
	{}
		SLATE_DEFAULT_SLOT(FArguments, Content)
		SLATE_EVENT(FOnClicked, OnReplayButtonClicked)
		SLATE_ARGUMENT(TWeakPtr<FMinesweeperGameSession>, GameSession)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
};