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
	/**
	 * \brief - Construct this decorator with the target matrix on which we want to add functionalities.
	 * \param[in] InMatrix - Target matrix to decorate.
	 */
	FMinesweeperMatrixNavigator(const TSharedRef<ICellMatrix<FMinesweeperCell>>& InMatrix);

	/**
	 * \brief - Get array of cell coordinates that are adjacent to certain cell coordinates within a certain square unit distance.
	 * \param[in] InCoordinates - Target coordinates for which we want to get the adjacent cell coordinates.
	 * \param[in] InSquareUnitDistance - Number of square distance of cells to search around for.
	 */
	TArray<FIntPoint> GetAdjacentsTo(const FMinesweeperCellCoordinate& InCoordinates, int InSquareUnitDistance = 1);

	/**
	 * \brief - Count the adjacent bombs to certain cell coordinates.
	 * \param[in] InCoordinates - Target coordinates for which we want to count the adjacent bombs.
	 * \param[in] InSquareUnitDistance - Number of square distance of cells to search around for.
	 */
	int CountAdjacentBombs(const FMinesweeperCellCoordinate& InCoordinates, int InSquareUnitDistance = 1);

private:
	/**
	 * \brief - Matrix on which this decorator adds functionalities.
	 */
	TWeakPtr<ICellMatrix<FMinesweeperCell>> _matrix;
};