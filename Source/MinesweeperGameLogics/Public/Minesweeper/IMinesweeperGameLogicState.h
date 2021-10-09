#pragma once

#include "CoreMinimal.h"

#include "FMinesweeperCellCoordinate.h"

/**
 * \brief - Interface handling the game logic state of a minesweeper game session
 */
struct MINESWEEPERGAMELOGICS_API IMinesweeperGameLogicState : public FAbstractLogicState {
	virtual ~IMinesweeperGameLogicState() = default;

	virtual void FlagOnCell(const FMinesweeperCellCoordinate& InCoordinates) = 0;
	virtual void SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates) = 0;

	TWeakPtr<FMinesweeperGameDataState> GameDataState;
	TWeakPtr<class FMinesweeperGameSession> GameSession;
};