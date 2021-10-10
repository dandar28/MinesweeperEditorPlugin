#pragma once

#include "CoreMinimal.h"

#include "CellMatrix/ICellMatrix.h"

#include "FMinesweeperCell.h"
#include "FMinesweeperCellCoordinate.h"

#include "CellMatrix/TMatrixNavigator.h"

/**
 * \brief - Structure for decorating a cell matrix with minesweeper cells to handle certain specific logics.
 */
class MINESWEEPERGAMELOGICS_API FMinesweeperMatrixNavigator
	: public TMatrixNavigator<FMinesweeperCell> {
public:
	/**
	 * \brief - Construct this decorator with the target matrix on which we want to add functionalities.
	 * \param[in] InMatrix - Target matrix to decorate.
	 */
	FMinesweeperMatrixNavigator(const TSharedRef<ICellMatrix<FMinesweeperCell>>& InMatrix);

	/**
	 * \brief - Count the adjacent bombs to certain cell coordinates.
	 * \param[in] InCoordinates - Target coordinates for which we want to count the adjacent bombs.
	 * \param[in] InSquareUnitDistance - Number of square distance of cells to search around for.
	 */
	int CountAdjacentBombs(const FMinesweeperCellCoordinate& InCoordinates, int InSquareUnitDistance = 1);

	/**
	 * \brief - Reveal all the adjacent empty cells around the one at the input coordinates, 
	 *			and do it recursively for each neighbour empty cell.
	 * \param[in] InCoordinates - Target coordinates from which we want to reveal the adjacent empty cells and recurse.
	 */
	void RevealAdjacentEmptyCellsRecursively(const FMinesweeperCellCoordinate& InCoordinates);
};