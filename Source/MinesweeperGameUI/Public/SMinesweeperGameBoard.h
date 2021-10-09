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

	/**
	 * \brief - Populate the grid matrix visually by regenerating it from the current state of the game session.
	 */
	void PopulateGrid();

	/**
	 * \brief - Run the selected action from the UI to the target coordinates.
	 * \param[in] InCoordinates - Target coordinates to run the action on.
	 */
	void RunAction(const FMinesweeperCellCoordinate& InCoordinates);

	/**
	 * \brief - When the selected action changes, update the selected action index.
	 */
	void OnSelectedActionChanged(TSharedPtr<FText> InNewItem, ESelectInfo::Type InSelectType);

	/**
	 * \return The selected action option string.
	 */
	FText GetSelectedActionOption() const;

private:

	/**
	 * \brief - Owned game session that is running on this game board UI widget.
	 */
	TSharedPtr<FMinesweeperGameSession> _gameSession;

	/**
	 * \brief - Grid panel holding the cells in a tabular organization.
	 */
	TSharedPtr<SUniformGridPanel> _cellsGridPanel;

	/**
	 * \brief - Array of options for the possible actions to be performed.
	 */
	const TArray<TSharedPtr<FText>> _actionComboOptions = TArray<TSharedPtr<FText>>{
		MakeShared<FText>(FText::FromString("Sweep")),
		MakeShared<FText>(FText::FromString("Flag"))
	};

	/**
	 * \brief - Function type which takes a game session and a cell coordinate to perform an action on it.
	 */
	using FActionFunction = TFunction<void(TSharedPtr<FMinesweeperGameSession>, const FMinesweeperCellCoordinate&)>;

	/**
	 * \brief - Array of ordered action functions for each option label of _actionComboOptions.
	 */
	const TArray<FActionFunction> _actionFunctions = TArray<FActionFunction>{
		[](auto InGameSession, const auto& InCoordinates) {
			InGameSession->SweepOnCell(InCoordinates);
		},
		[](auto InGameSession, const auto& InCoordinates) {
			InGameSession->FlagOnCell(InCoordinates);
		}
	};

	/**
	 * \brief - Currently selected action index.
	 */
	int32 _selectedActionIndex = 0;
};