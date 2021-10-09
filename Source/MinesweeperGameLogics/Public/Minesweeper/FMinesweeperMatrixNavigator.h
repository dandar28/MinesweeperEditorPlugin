#pragma once

#include "CoreMinimal.h"

#include "CellMatrix/ICellMatrix.h"

#include "FMinesweeperCell.h"
#include "FMinesweeperCellCoordinate.h"

#include "CellMatrix/TMatrixNavigator.h"

/**
 * \brief - Structure for decorating a cell matrix with minesweeper cells to handle certain specific logics.
 */
struct MINESWEEPERGAMELOGICS_API FMinesweeperMatrixNavigator {
	FMinesweeperMatrixNavigator(const TSharedRef<ICellMatrix<FMinesweeperCell>>& matrix);

	TArray<FIntPoint> GetAdjacentsTo(const FMinesweeperCellCoordinate& InCoordinates, int InSquareUnitDistance = 1);

	int CountAdjacentBombs(const FMinesweeperCellCoordinate& InCoordinates, int InSquareUnitDistance = 1);

private:
	TWeakPtr<ICellMatrix<FMinesweeperCell>> _matrix;
};