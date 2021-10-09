#pragma once

#include "Minesweeper/IMinesweeperGameLogicState.h"

struct MINESWEEPERGAMELOGICS_API FIdleLogicState : public IMinesweeperGameLogicState {
	virtual ~FIdleLogicState() = default;

	void FlagOnCell(const FMinesweeperCellCoordinate& InCoordinates) override {}
	void SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates) override {}
};