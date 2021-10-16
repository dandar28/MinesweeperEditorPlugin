#pragma once

#include "Minesweeper/IMinesweeperGameLogicState.h"

/**
 * \brief - Logic State that handles the actual playing of minesweeper.
 */
class MINESWEEPERGAMELOGICS_API FPlayingLogicState
	: public IMinesweeperGameLogicState {
public:
	virtual ~FPlayingLogicState() = default;

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