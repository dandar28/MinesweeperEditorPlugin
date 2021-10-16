#pragma once

#include "Minesweeper/IMinesweeperGameLogicState.h"

/**
 * \brief - Mock Logic State that does nothing and is only used for idle state.
 */
class MINESWEEPERGAMELOGICS_API FIdleLogicState
	: public IMinesweeperGameLogicState {
public:
	virtual ~FIdleLogicState() = default;

	void RunAction(TSharedRef<IMinesweeperAction> InAction, const FMinesweeperCellCoordinate& InCoordinates) override {}
};