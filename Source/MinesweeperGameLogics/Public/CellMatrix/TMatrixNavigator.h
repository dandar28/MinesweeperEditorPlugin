#pragma once

#include "CoreMinimal.h"

#include "ICellMatrix.h"

template <typename CellType>
struct MINESWEEPERGAMELOGICS_API TMatrixNavigator {
	TMatrixNavigator(const TSharedRef<ICellMatrix<CellType>>& matrix) {
		_matrix = matrix;
	}

	TArray<FIntPoint> GetAdjacentsTo(const FIntPoint& InCoordinates, int InSquareUnitDistance = 1) {
		check(_matrix.IsValid());
		auto Matrix = _matrix.Pin();
		TArray<FIntPoint> AdjacentCells;
		for (int CurrentColumn = -InSquareUnitDistance; CurrentColumn <= InSquareUnitDistance; CurrentColumn++) {
			for (int CurrentRow = -InSquareUnitDistance; CurrentRow <= InSquareUnitDistance; CurrentRow++) {
				if (CurrentColumn == 0 && CurrentRow == 0) {
					continue;
				}
				FIntPoint AdjacentCoordinates(InCoordinates + FIntPoint(CurrentColumn, CurrentRow));
				if (Matrix->Has(AdjacentCoordinates)) {
					AdjacentCells.Add(AdjacentCoordinates);
				}
			}
		}
		return AdjacentCells;
	}

private:
	TWeakPtr<ICellMatrix<CellType>> _matrix;
};