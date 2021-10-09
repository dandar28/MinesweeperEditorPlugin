#pragma once

#include "Minesweeper/IMinesweeperGameLogicState.h"

struct MINESWEEPERGAMELOGICS_API FPlayingLogicState : public IMinesweeperGameLogicState {
	virtual ~FPlayingLogicState() = default;

	void FlagOnCell(const FMinesweeperCellCoordinate& InCoordinates) override;
	void SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates) override;

private:
	void _revealAdjacents(const FMinesweeperCellCoordinate& InCoordinates);
};