#include "Minesweeper/FMinesweeperMatrixNavigator.h"

#include "Minesweeper/EMinesweeperCellFlags.h"
#include "Minesweeper/EMinesweeperCellState.h"
#include "Minesweeper/FMinesweeperCell.h"
#include "Minesweeper/FMinesweeperCellCoordinate.h"

inline FMinesweeperMatrixNavigator::FMinesweeperMatrixNavigator(const TSharedRef<ICellMatrix<FMinesweeperCell>>& matrix) {
	_matrix = matrix;
}

inline TArray<FIntPoint> FMinesweeperMatrixNavigator::GetAdjacentsTo(const FMinesweeperCellCoordinate& InCoordinates, int InSquareUnitDistance) {
	return TMatrixNavigator<FMinesweeperCell>(_matrix.Pin().ToSharedRef()).GetAdjacentsTo(InCoordinates, InSquareUnitDistance);
}

inline int FMinesweeperMatrixNavigator::CountAdjacentBombs(const FMinesweeperCellCoordinate& InCoordinates, int InSquareUnitDistance) {
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
