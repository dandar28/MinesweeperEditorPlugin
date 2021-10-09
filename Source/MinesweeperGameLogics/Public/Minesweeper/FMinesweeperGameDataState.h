#pragma once

#include "CoreMinimal.h"

#include "CellMatrix/ICellMatrix.h"

#include "Minesweeper/FMinesweeperCell.h"

using FMinesweeperMatrix = TSharedPtr<ICellMatrix<FMinesweeperCell>>;

/**
 * \brief - Class holding the game data state of a minesweeper game session
 */
class MINESWEEPERGAMELOGICS_API FMinesweeperGameDataState {
public:
	FMinesweeperMatrix Matrix;

	void RebuildMatrix(int InWidth, int InHeight);

	void ForeachCell(const TFunction<void(FMinesweeperCell&)>& InPredicate);

	void RevealAllCells();
	void ClearMatrixCells();
	void ClearAndPlaceRandomMines(int InNumberOfMines);
};