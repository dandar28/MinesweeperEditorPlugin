#pragma once

#include "Minesweeper/IMinesweeperGameLogicState.h"

/**
 * \brief - Logic State used for ending the game in case of a game over.
 */
class MINESWEEPERGAMELOGICS_API FGameOverLogicState
	: public IMinesweeperGameLogicState {
public:
	virtual ~FGameOverLogicState() = default;

	void FlagOnCell(const FMinesweeperCellCoordinate& InCoordinates) override {}
	void SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates) override {}

	/**
	 * \brief - When entering the game over state, broadcast OnGameOver event and reveal all cells.
	 */
	void OnEnter() override;
};