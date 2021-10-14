#pragma once

#include "CoreMinimal.h"

#include "CellMatrix/ICellMatrix.h"

#include "Minesweeper/FTickTimer.h"
#include "Minesweeper/FMinesweeperCell.h"
#include "Minesweeper/FMinesweeperCellCoordinate.h"

struct IMinesweeperAction {
	virtual void Perform(const TSharedRef<class FMinesweeperGameSession> InGameSession, const FMinesweeperCellCoordinate& InCoordinates) = 0;
};

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

struct FRegisteredAction {
	FDateTime Time;
	TSharedPtr<IMinesweeperAction> Action;
	FMinesweeperCellCoordinate InteractedCell;
};

struct FActionHistory {
	TArray<FRegisteredAction> Actions;
};