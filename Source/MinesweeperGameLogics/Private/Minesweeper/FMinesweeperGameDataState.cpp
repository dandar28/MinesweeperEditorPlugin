#include "Minesweeper/FMinesweeperGameDataState.h"

#include "CellMatrix/ICellMatrix.h"
#include "CellMatrix/TCellMatrix.h"

#include "Minesweeper/FMinesweeperCellCoordinate.h"
#include "Minesweeper/FMinesweeperCell.h"

void FMinesweeperGameDataState::RebuildMatrix(int InWidth, int InHeight) {
	Matrix = StaticCastSharedRef<ICellMatrix<FMinesweeperCell>>(MakeShared<TCellMatrix<FMinesweeperCell>>(InWidth, InHeight));

	ClearMatrixCells();
}

void FMinesweeperGameDataState::ForeachCell(const TFunction<void(const FMinesweeperCellCoordinate&, FMinesweeperCell&)>& InPredicate) {
	check(!!InPredicate);
	check(Matrix.IsValid());

	// Iterate all cell coordinates and, for each retrieved cell reference, call the input predicate function.
	for (int CurrentColumn = 0; CurrentColumn < Matrix->GetSize().X; CurrentColumn++) {
		for (int CurrentRow = 0; CurrentRow < Matrix->GetSize().Y; CurrentRow++) {
			FMinesweeperCellCoordinate CurrentCellCoordinate(CurrentColumn, CurrentRow);
			FMinesweeperCell& CurrentCell = Matrix->Get(CurrentCellCoordinate);
			InPredicate(CurrentCellCoordinate, CurrentCell);
		}
	}
}

void FMinesweeperGameDataState::RevealAllCells() {
	ForeachCell([](const FMinesweeperCellCoordinate&, FMinesweeperCell& InRefCurrentCell) {
		InRefCurrentCell.SetRevealed(true);
	});
}

void FMinesweeperGameDataState::ClearMatrixCells() {
	ForeachCell([](const FMinesweeperCellCoordinate&, FMinesweeperCell& InRefCurrentCell) {
		InRefCurrentCell = FMinesweeperCell();
	});
}

void FMinesweeperGameDataState::ClearAndPlaceRandomMines(int InNumberOfMines) {
	check(Matrix.IsValid());

	ClearMatrixCells();

	const int NumberOfCells = Matrix->GetNumberOfCells();
	check(NumberOfCells > 0);
	check(InNumberOfMines <= NumberOfCells);

	// Populate an array of coordinates that represent the free cells of the matrix.
	TArray<FMinesweeperCellCoordinate> FreeCells;
	ForeachCell([&FreeCells](const FMinesweeperCellCoordinate& InCurrentCellCoordinate, FMinesweeperCell&) {
		FreeCells.Add(InCurrentCellCoordinate);
	});

	// Place the mines on randomly picked free cells' coordinates.
	for (int MineIndex = 0; MineIndex < InNumberOfMines; MineIndex++) {
		FMinesweeperCellCoordinate PickedFreeCell = FreeCells[FMath::RandRange(0, FreeCells.Num() - 1)];
		Matrix->Get(PickedFreeCell).CellState = EMinesweeperCellState::Bomb;
	}
}