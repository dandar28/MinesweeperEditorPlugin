#pragma once

#include "Minesweeper/IMinesweeperGameLogicState.h"

/**
 * \brief - Logic State that handles the actual playing of minesweeper.
 */
class MINESWEEPERGAMELOGICS_API FPlayingLogicState
	: public IMinesweeperGameLogicState {
public:
	virtual ~FPlayingLogicState() = default;

	/**
	 * \brief - Run a generic action on certain input coordinates.
	 * \param[in] InAction - Action to be executed.
	 * \param[in] InCoordinates - Target coordinates to execute the action on.
	 */
	void RunAction(TSharedRef<IMinesweeperAction> InAction, const FMinesweeperCellCoordinate& InCoordinates) override;

	/**
	 * \brief - When entering the game playing-logic, we want to start the timer and prepare the gameplay state.
	 */
	void OnEnter() override;

	/**
	 * \brief - When exiting the game playing-logic, we want to stop the timer and reset the gameplay state.
	 */
	void OnExit() override;
};