#pragma once

#include "CoreMinimal.h"

#include "CellMatrix/ICellMatrix.h"

#include "FMinesweeperCell.h"
#include "FMinesweeperCellCoordinate.h"

#include "CellMatrix/TMatrixNavigator.h"

struct MINESWEEPERGAMELOGICS_API FMinesweeperMatrixNavigator {
	FMinesweeperMatrixNavigator(const TSharedRef<ICellMatrix<FMinesweeperCell>>& matrix) {
		_matrix = matrix;
	}

	TArray<FIntPoint> GetAdjacentsTo(const FMinesweeperCellCoordinate& InCoordinates, int InSquareUnitDistance = 1) {
		return TMatrixNavigator<FMinesweeperCell>(_matrix.Pin().ToSharedRef()).GetAdjacentsTo(InCoordinates, InSquareUnitDistance);
	}

	int CountAdjacentBombs(const FMinesweeperCellCoordinate& InCoordinates, int InSquareUnitDistance = 1) {
		const auto Matrix = _matrix.Pin();
		const auto AdjacentCellsCoordinates = GetAdjacentsTo(InCoordinates, InSquareUnitDistance);

		int NumberOfAdjacentBombs = 0;
		for (const auto AdjacentCellCoordinates : AdjacentCellsCoordinates) {
			if (Matrix->Get(AdjacentCellCoordinates).CellState == EMinesweeperCellState::Bomb) {
				NumberOfAdjacentBombs++;
			}
		}

		return NumberOfAdjacentBombs;
	}

private:
	TWeakPtr<ICellMatrix<FMinesweeperCell>> _matrix;
};