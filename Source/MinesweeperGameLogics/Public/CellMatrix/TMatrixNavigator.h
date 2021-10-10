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
	 * \brief - Run a predicate function for each cell coordinate.
	 * \param[in] InPredicate - Predicate function to execute.
	 */
	void ForeachCellCoordinate(const TFunction<void(const FIntPoint& InCoordinates)>& InPredicate) {
		check(!!InPredicate);
		check(_matrix.IsValid());

		const auto MatrixSize = _matrix.Pin()->GetSize();

		// Iterate all cell coordinates and, for each retrieved cell reference, call the input predicate function.
		for (int CurrentColumn = 0; CurrentColumn < MatrixSize.X; CurrentColumn++) {
			for (int CurrentRow = 0; CurrentRow < MatrixSize.Y; CurrentRow++) {
				FIntPoint CurrentCellCoordinate(CurrentColumn, CurrentRow);
				InPredicate(CurrentCellCoordinate);
			}
		}
	}

	/**
	 * \brief - Run a predicate function for each cell and coordinate.
	 * \param[in] InPredicate - Predicate function to execute.
	 */
	void ForeachCell(const TFunction<void(const FIntPoint& InCoordinates, CellType& InRefCell)>& InPredicate) {
		const auto Matrix = _matrix.Pin();
		ForeachCellCoordinate([Matrix, InPredicate](const FIntPoint& InCoordinates) {
			InPredicate(InCoordinates, Matrix->Get(InCoordinates));
		});
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

		// Iterate bidimensionally around a square with a side of InSquareUnitDistance on all directions.
		for (int CurrentColumn = -InSquareUnitDistance; CurrentColumn <= InSquareUnitDistance; CurrentColumn++) {
			for (int CurrentRow = -InSquareUnitDistance; CurrentRow <= InSquareUnitDistance; CurrentRow++) {
				if (CurrentColumn == 0 && CurrentRow == 0) {
					// Ignore the center cell since it would be the one at the input coordinates.
					continue;
				}

				// Obtain the adjacent cell's coordinates.
				FIntPoint AdjacentCoordinates(InCoordinates + FIntPoint(CurrentColumn, CurrentRow));
				if (Matrix->Has(AdjacentCoordinates)) {
					// If the cell exists, add it to the array of adjacent cells.
					AdjacentCells.Add(AdjacentCoordinates);
				}
			}
		}
		return AdjacentCells;
	}

protected:
	/**
	 * \brief - Matrix on which this decorator adds functionalities.
	 */
	TWeakPtr<ICellMatrix<CellType>> _matrix;
};