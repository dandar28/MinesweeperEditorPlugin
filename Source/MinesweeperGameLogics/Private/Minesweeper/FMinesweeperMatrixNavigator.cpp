#include "Minesweeper/FMinesweeperMatrixNavigator.h"

#include "Minesweeper/EMinesweeperCellFlags.h"
#include "Minesweeper/EMinesweeperCellState.h"
#include "Minesweeper/FMinesweeperCell.h"
#include "Minesweeper/FMinesweeperCellCoordinate.h"

inline FMinesweeperMatrixNavigator::FMinesweeperMatrixNavigator(const TSharedRef<ICellMatrix<FMinesweeperCell>>& InMatrix) {
	_matrix = InMatrix;
}

inline TArray<FIntPoint> FMinesweeperMatrixNavigator::GetAdjacentsTo(const FMinesweeperCellCoordinate& InCoordinates, int InSquareUnitDistance) {
	return TMatrixNavigator<FMinesweeperCell>(_matrix.Pin().ToSharedRef()).GetAdjacentsTo(InCoordinates, InSquareUnitDistance);
}

inline int FMinesweeperMatrixNavigator::CountAdjacentBombs(const FMinesweeperCellCoordinate& InCoordinates, int InSquareUnitDistance) {
	const auto Matrix = _matrix.Pin();

	// Counter to be incremented for each adjacent bomb with respect to the input coordinates' cell.
	int NumberOfAdjacentBombs = 0;

	// Retrieve the adjacent cells to the input coordinates within the input square unit distance provided.
	const auto AdjacentCellsCoordinates = GetAdjacentsTo(InCoordinates, InSquareUnitDistance);

	// For each adjacent cell that is a Bomb, increment a counter of adjacent cells.
	for (const auto AdjacentCellCoordinates : AdjacentCellsCoordinates) {
		if (Matrix->Get(AdjacentCellCoordinates).CellState == EMinesweeperCellState::Bomb) {
			NumberOfAdjacentBombs++;
		}
	}

	return NumberOfAdjacentBombs;
}
