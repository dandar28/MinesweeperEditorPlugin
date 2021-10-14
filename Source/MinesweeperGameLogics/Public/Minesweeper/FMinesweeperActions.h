#pragma once

#include "CoreMinimal.h"

#include "CellMatrix/ICellMatrix.h"

#include "Minesweeper/FTickTimer.h"
#include "Minesweeper/FMinesweeperCell.h"
#include "Minesweeper/FMinesweeperCellCoordinate.h"

/**
 * \brief - Interface for generic minesweeper actions that can be performed with respect to a game session on a certain cell coordinate.
 */
struct IMinesweeperAction {
	virtual void Perform(const TSharedRef<class FMinesweeperGameSession> InGameSession, const FMinesweeperCellCoordinate& InCoordinates) = 0;
};

/**
 * \brief - Static accessor for action implementations.
 */
struct FMinesweeperActions {
	//<<< Publicly accessible implemented actions
	static TSharedRef<IMinesweeperAction> Sweep;
	static TSharedRef<IMinesweeperAction> Flag;
	static TSharedRef<IMinesweeperAction> Mark;
	//>>>

	//<<< Private Implementations
	class FMinesweeperActionSweep;
	class FMinesweeperActionFlag;
	class FMinesweeperActionMark;
	//>>>
};

/**
 * \brief - Structure for an action that has been registered, in order to be able to add it to an history.
 */
struct FRegisteredAction {
	FDateTime Time;
	TSharedPtr<IMinesweeperAction> Action;
	FMinesweeperCellCoordinate InteractedCell;
};

/**
 * \brief - An history of registered actions.
 */
struct FActionHistory {
	TArray<FRegisteredAction> Actions;
};