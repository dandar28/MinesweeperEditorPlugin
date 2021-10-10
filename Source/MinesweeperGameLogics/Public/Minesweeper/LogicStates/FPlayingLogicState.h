#pragma once

#include "Minesweeper/IMinesweeperGameLogicState.h"

/**
 * \brief - Logic State that handles the actual playing of minesweeper.
 */
class MINESWEEPERGAMELOGICS_API FPlayingLogicState
	: public IMinesweeperGameLogicState {
public:
	virtual ~FPlayingLogicState() = default;

	void FlagOnCell(const FMinesweeperCellCoordinate& InCoordinates) override;
	void SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates) override;
};