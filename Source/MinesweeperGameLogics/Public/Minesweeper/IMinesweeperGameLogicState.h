#pragma once

#include "CoreMinimal.h"

#include "StateMachine/FAbstractLogicState.h"

#include "FMinesweeperCellCoordinate.h"

/**
 * \brief - Interface representing a game logic state of a minesweeper game session.
 */
class MINESWEEPERGAMELOGICS_API IMinesweeperGameLogicState
	: public FAbstractLogicState {
public:
	virtual ~IMinesweeperGameLogicState() = default;

	virtual void FlagOnCell(const FMinesweeperCellCoordinate& InCoordinates) = 0;
	virtual void SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates) = 0;

	/**
	 * \brief - Data state of the related Minesweeper game session.
	 */
	TWeakPtr<FMinesweeperGameDataState> GameDataState;

	/**
	 * \brief - Owning Minesweeper game session on which this state interacts.
	 */
	TWeakPtr<class FMinesweeperGameSession> GameSession;
};