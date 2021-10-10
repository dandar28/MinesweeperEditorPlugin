#pragma once

#include "CoreMinimal.h"

#include "CellMatrix/ICellMatrix.h"

#include "Minesweeper/FMinesweeperCell.h"
#include "Minesweeper/FMinesweeperCellCoordinate.h"

using FMinesweeperMatrix = TSharedPtr<ICellMatrix<FMinesweeperCell>>;

/**
 * \brief - Class holding the game data state of a minesweeper game session
 */
class MINESWEEPERGAMELOGICS_API FMinesweeperGameDataState {
public:
	FMinesweeperMatrix Matrix;

	/**
	 * \brief - Rebuild the matrix instance with a new size and clear all cells to default cell instance state.
	 * \param[in] InWidth - Matrix width of cells.
	 * \param[in] InHeight - Matrix height of cells.
	 */
	void RebuildMatrix(int InWidth, int InHeight);

	/**
	 * \brief - Execute a predicate function for each cell of the matrix.
	 * param[in] InPredicate - Predicate function to be runned on each cell which takes a cell reference as input argument.
	 */
	void ForeachCell(const TFunction<void(const FMinesweeperCellCoordinate&, FMinesweeperCell&)>& InPredicate);

	/**
	 * \brief - Reveal all the cells of the matrix.
	 */
	void RevealAllCells();

	/**
	 * \brief - Reset all the cells to the default cell instance state.
	 */
	void ClearMatrixCells();

	/**
	 * \brief - Clear all matrix cells and place random mines around the matrix.
	 * \param[in] InNumberOfMines - Number of mines to be randomly placed around the matrix.
	 */
	void ClearAndPlaceRandomMines(int InNumberOfMines);
};