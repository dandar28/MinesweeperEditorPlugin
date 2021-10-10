#include "Minesweeper/FMinesweeperGameDataState.h"

#include "CellMatrix/ICellMatrix.h"
#include "CellMatrix/TCellMatrix.h"

#include "Minesweeper/FMinesweeperCellCoordinate.h"
#include "Minesweeper/FMinesweeperCell.h"
#include "Minesweeper/FMinesweeperMatrixNavigator.h"

void FMinesweeperGameDataState::RebuildMatrix(int InWidth, int InHeight) {
	Matrix = StaticCastSharedRef<ICellMatrix<FMinesweeperCell>>(MakeShared<TCellMatrix<FMinesweeperCell>>(InWidth, InHeight));

	ClearMatrixCells();
}

void FMinesweeperGameDataState::ForeachCell(const TFunction<void(const FMinesweeperCellCoordinate&, FMinesweeperCell&)>& InPredicate) const {
	check(!!InPredicate);
	check(Matrix.IsValid());

	FMinesweeperMatrixNavigator(Matrix.ToSharedRef()).ForeachCell(InPredicate);
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

bool FMinesweeperGameDataState::IsGameWon() const {
	int CountBombs = 0;
	int CountRevealed = 0;
	int CountCells = 0;

	ForeachCell([&CountRevealed, &CountBombs, &CountCells](const FMinesweeperCellCoordinate&, FMinesweeperCell& InRefCurrentCell) {
		if (InRefCurrentCell.IsRevealed()) {
			CountRevealed++;
		}

		if (InRefCurrentCell.CellState == EMinesweeperCellState::Bomb) {
			CountBombs++;
		}

		CountCells++;
	});

	check(CountCells == Matrix->GetNumberOfCells());

	const bool bRevealedAllCellsExceptBombs = (CountRevealed == (CountCells - CountBombs));
	return bRevealedAllCellsExceptBombs;
}