#pragma once

#include "CoreMinimal.h"

#include "ICellMatrix.h"

/**
 * \brief - Templated decorator class which, given a cell matrix of any cell type, adds certain easing methods.
 */
template <typename CellType>
class MINESWEEPERGAMELOGICS_API TMatrixNavigator {
public:
	/**
	 * \brief - Construct this decorator with the target matrix on which we want to add functionalities.
	 * \param[in] InMatrix - Target matrix to decorate.
	 */
	TMatrixNavigator(const TSharedRef<ICellMatrix<CellType>>& InMatrix) {
		_matrix = InMatrix;
	}

	/**
	 * \brief - Get array of cell coordinates that are adjacent to certain cell coordinates within a certain square unit distance.
	 * \param[in] InCoordinates - Target coordinates for which we want to get the adjacent cell coordinates.
	 * \param[in] InSquareUnitDistance - Number of square distance of cells to search around for.
	 */
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
	/**
	 * \brief - Matrix on which this decorator adds functionalities.
	 */
	TWeakPtr<ICellMatrix<CellType>> _matrix;
};