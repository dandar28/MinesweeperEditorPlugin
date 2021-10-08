#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

#include "MinesweeperGameSession.h"

class MINESWEEPERGAMEUI_API SMinesweeperGameBoard : public SCompoundWidget {
public:

	SLATE_BEGIN_ARGS(SMinesweeperGameBoard)
		: _GameSession(nullptr)
	{}

		SLATE_ARGUMENT( TSharedPtr<FMinesweeperGameSession>, GameSession )
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	bool SupportsKeyboardFocus() const override;

	void PopulateGrid();

private:

	TSharedPtr<FMinesweeperGameSession> _gameSession;
	TSharedPtr<SUniformGridPanel> _cellsGridPanel;
};