#include "Minesweeper/FMinesweeperMatrixNavigator.h"

#include "Minesweeper/EMinesweeperCellFlags.h"
#include "Minesweeper/EMinesweeperCellState.h"
#include "Minesweeper/FMinesweeperCell.h"
#include "Minesweeper/FMinesweeperCellCoordinate.h"

FMinesweeperMatrixNavigator::FMinesweeperMatrixNavigator(const TSharedRef<ICellMatrix<FMinesweeperCell>>& InMatrix) {
	_matrix = InMatrix;
}

TArray<FIntPoint> FMinesweeperMatrixNavigator::GetAdjacentsTo(const FMinesweeperCellCoordinate& InCoordinates, int InSquareUnitDistance) {
	return TMatrixNavigator<FMinesweeperCell>(_matrix.Pin().ToSharedRef()).GetAdjacentsTo(InCoordinates, InSquareUnitDistance);
}

int FMinesweeperMatrixNavigator::CountAdjacentBombs(const FMinesweeperCellCoordinate& InCoordinates, int InSquareUnitDistance) {
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

void FMinesweeperMatrixNavigator::RevealAdjacentEmptyCellsRecursively(const FMinesweeperCellCoordinate& InCoordinates) {
	const auto Matrix = _matrix.Pin();

	// Count the adjacent bombs to the input coordinates.
	const int NumOfAdjacentBombs = CountAdjacentBombs(InCoordinates);

	// If the input cell has at least one adjacent bomb, stop revealing adjacent empty cells.
	if (NumOfAdjacentBombs > 0) {
		return;
	}

	TArray<FIntPoint> AdjacentCellsCoordinates = GetAdjacentsTo(InCoordinates);
	for (const auto& AdjacentCellCoordinates : AdjacentCellsCoordinates) {
		if (!Matrix->Has(AdjacentCellCoordinates)) {
			continue;
		}

		// For each adjacent cell that has not been revealed yet and it is empty, reveal it.
		auto& AdjacentCell = Matrix->Get(AdjacentCellCoordinates);
		if (!AdjacentCell.IsRevealed() && AdjacentCell.CellState == EMinesweeperCellState::Empty) {
			AdjacentCell.SetRevealed(true);

			// If the revealed empty cell has no adjacent bombs, reveal also its adjacent empty spaces by recursing on it.
			if (CountAdjacentBombs(AdjacentCellCoordinates) == 0) {
				RevealAdjacentEmptyCellsRecursively(AdjacentCellCoordinates);
			}
		}
	}
}