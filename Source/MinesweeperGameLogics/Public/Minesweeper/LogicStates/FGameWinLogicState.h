#pragma once

#include "Minesweeper/IMinesweeperGameLogicState.h"

/**
 * \brief - Logic State used for ending the game in case of a game win.
 */
class MINESWEEPERGAMELOGICS_API FGameWinLogicState
	: public IMinesweeperGameLogicState {
public:
	virtual ~FGameWinLogicState() = default;

	void FlagOnCell(const FMinesweeperCellCoordinate& InCoordinates) override {}
	void SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates) override {}

	/**
	 * \brief - When entering the game win state, broadcast OnGameWin event and reveal all cells.
	 */
	void OnEnter() override;
};