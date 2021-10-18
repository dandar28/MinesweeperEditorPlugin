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

	/**
	 * \brief - Method to be implemented for taking care of running a generic action on certain input coordinates.
	 * \param[in] InAction - Action to be executed.
	 * \param[in] InCoordinates - Target coordinates to execute the action on.
	 */
	virtual void RunAction(TSharedRef<IMinesweeperAction> InAction, const FMinesweeperCellCoordinate& InCoordinates) = 0;

	/**
	 * \brief - Data state of the related Minesweeper game session.
	 */
	TWeakPtr<FMinesweeperGameDataState> GameDataState;

	/**
	 * \brief - Owning Minesweeper game session on which this state interacts.
	 */
	TWeakPtr<class FMinesweeperGameSession> GameSession;
};