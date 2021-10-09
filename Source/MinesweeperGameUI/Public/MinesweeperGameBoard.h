#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

#include "Minesweeper/FMinesweeperGameSession.h"

/**
 * \brief - Slate widget that renders a minesweeper game board visually and handles the
 *			game logic bindings with the UI updates through an instanced game session.
 */
class MINESWEEPERGAMEUI_API SMinesweeperGameBoard
	: public SCompoundWidget {
public:

	SLATE_BEGIN_ARGS(SMinesweeperGameBoard)
		: _GameSession(nullptr)
	{}

		SLATE_ARGUMENT( TSharedPtr<FMinesweeperGameSession>, GameSession )
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	bool SupportsKeyboardFocus() const override;

	void PopulateGrid();

	void RunAction(const FMinesweeperCellCoordinate& InCoordinates);

	void OnSelectedActionChanged(TSharedPtr<FText> InNewItem, ESelectInfo::Type InSelectType);
	FText GetSelectedActionOption() const;

private:

	TSharedPtr<FMinesweeperGameSession> _gameSession;
	TSharedPtr<SUniformGridPanel> _cellsGridPanel;

	const TArray<TSharedPtr<FText>> _actionComboOptions = TArray<TSharedPtr<FText>>{
		MakeShared<FText>(FText::FromString("Sweep")),
		MakeShared<FText>(FText::FromString("Flag"))
	};

	using FActionFunction = TFunction<void(TSharedPtr<FMinesweeperGameSession>, const FMinesweeperCellCoordinate&)>;
	const TArray<FActionFunction> _actionFunctions = TArray<FActionFunction>{
		[](auto InGameSession, const auto& InCoordinates) {
			InGameSession->SweepOnCell(InCoordinates);
		},
		[](auto InGameSession, const auto& InCoordinates) {
			InGameSession->FlagOnCell(InCoordinates);
		}
	};

	int32 _selectedActionIndex = 0;
};