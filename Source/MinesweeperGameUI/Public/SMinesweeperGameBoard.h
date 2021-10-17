#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

#include "SNumericSettingEntry.h"
#include "SMinesweeperCell.h"

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
	 * \brief - Startup the game with the current game settings chosen through the UI.
	 */
	void StartGameWithCurrentSettings();

	/**
	 * \brief - Populate the grid matrix visually by regenerating it from the current state of the game session.
	 */
	void PopulateGrid();

	/**
	 * \brief - Run the selected action from the UI to the target coordinates.
	 * \param[in] InCoordinates - Target coordinates to run the action on.
	 */
	void RunAction(TSharedRef<IMinesweeperAction> InAction, const FMinesweeperCellCoordinate& InCoordinates);

private:

	/**
	 * \brief - This methods shows and plays the Replay of the actions history so far.
	 */
	void _executeReplay();

	/**
	 * \brief - Owned game session that is running on this game board UI widget.
	 */
	TSharedPtr<FMinesweeperGameSession> _gameSession;

	/**
	 * \brief - Grid panel holding the cells in a tabular organization.
	 */
	TSharedPtr<SUniformGridPanel> _cellsGridPanel;

	/**
	 * \brief - Last play's time start.
	 */
	FDateTime _timeStart;

	/**
	 * \brief - Existing replaying display executions.
	 */
	TSet<FGuid> _replayExecutions;

	/**
	 * \brief - Critical section for mutex access to replay method call.
	 */
	FCriticalSection _mutexReplay;

	/**
	 * \brief - When this is set to true, existing replaying displays are stopped.
	 */
	FThreadSafeBool _bShouldStopReplay = false;

	/**
	 * \brief - Shared pointer to the actual instance of the game settings updated from the UI.
	 */
	TSharedPtr<FMinesweeperGameSettings> _gameSettings;
};