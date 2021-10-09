#pragma once

#include "Minesweeper/IMinesweeperGameLogicState.h"

struct MINESWEEPERGAMELOGICS_API FGameOverLogicState : public IMinesweeperGameLogicState {
	virtual ~FGameOverLogicState() = default;

	void FlagOnCell(const FMinesweeperCellCoordinate& InCoordinates) override {}
	void SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates) override {}

	void OnEnter() override;
};