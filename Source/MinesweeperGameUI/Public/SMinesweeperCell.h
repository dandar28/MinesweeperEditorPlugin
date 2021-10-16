#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

#include "Minesweeper/FMinesweeperGameSession.h"

/**
 * \brief - Structure which represent the style data for a cell.
 */
struct FCellStyle {
	FCellStyle& SetTextColor(const FLinearColor& InColor) {
		TextColor = InColor;
		return *this;
	}
	FCellStyle& SetBackgroundColor(const FLinearColor& InColor) {
		BackgroundColor = InColor;
		return *this;
	}
	FCellStyle& SetText(const FString& InText) {
		Text = InText;
		return *this;
	}
	FCellStyle& SetContentWidget(const TSharedPtr<SWidget> InWidget) {
		ContentWidget = InWidget;
		return *this;
	}

	TSharedPtr<SWidget> ContentWidget;
	FLinearColor TextColor = FLinearColor::Black;
	FLinearColor BackgroundColor = FLinearColor::White;
	FString Text;
};

DECLARE_DELEGATE_RetVal_TwoParams(FReply, FOnMinesweeperCellAction,
	const FMinesweeperCellCoordinate&,
	const FMinesweeperCell&)

/**
 * \brief - Slate widget that renders a minesweeper game board visually and handles the
 *			game logic bindings with the UI updates through an instanced game session.
 */
class MINESWEEPERGAMEUI_API SMinesweeperCell
	: public SCompoundWidget {
public:

	SLATE_BEGIN_ARGS(SMinesweeperCell)
	{}
		SLATE_ARGUMENT(FMinesweeperCellCoordinate, CellCoordinates)
		SLATE_ARGUMENT(FMinesweeperCell, Cell)
		SLATE_ARGUMENT(TSharedPtr<FMinesweeperGameSession>, GameSession)

		SLATE_EVENT(FOnMinesweeperCellAction, OnLeftClicked)
		SLATE_EVENT(FOnMinesweeperCellAction, OnMiddleClicked)
		SLATE_EVENT(FOnMinesweeperCellAction, OnRightClicked)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	FOnMinesweeperCellAction OnLeftClicked;
	FOnMinesweeperCellAction OnMiddleClicked;
	FOnMinesweeperCellAction OnRightClicked;

private:

	FCellStyle _getEmptyCellStyle(const FMinesweeperCellCoordinate& InCellCoordinates) const;

	static TArray<FCellStyle> _emptyCellStylePerBombsAdjacencyCount;
	static FCellStyle _hiddenCellStyle;
	static FCellStyle _bombCellStyle;
	static FCellStyle _flagCellStyle;
	static FCellStyle _questionMarkCellStyle;

	static FLinearColor _emptyCellColorIntensityDark;
	static FLinearColor _emptyCellColorIntensityLight;
	
	/**
	 * \brief - Owned game session that is running on this game board UI widget.
	 */
	TSharedPtr<FMinesweeperGameSession> _gameSession;

	FMinesweeperCellCoordinate _cellCoordinates;
	FMinesweeperCell _cell;
};