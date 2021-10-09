#pragma once

#include "CoreMinimal.h"

#include "ICellMatrix.h"

/**
 * \brief - Templated decorator class which, given a cell matrix of any cell type, adds certain easing methods.
 */
template <typename CellType>
class MINESWEEPERGAMELOGICS_API TMatrixNavigator {
public:
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