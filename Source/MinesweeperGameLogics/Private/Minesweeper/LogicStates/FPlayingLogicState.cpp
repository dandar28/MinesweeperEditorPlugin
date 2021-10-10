#include "Minesweeper/LogicStates/FPlayingLogicState.h"

#include "Minesweeper/EMinesweeperCellState.h"
#include "Minesweeper/FMinesweeperCell.h"
#include "Minesweeper/FMinesweeperMatrixNavigator.h"
#include "Minesweeper/FMinesweeperGameSession.h"
#include "Minesweeper/FMinesweeperGameDataState.h"

void FPlayingLogicState::FlagOnCell(const FMinesweeperCellCoordinate& InCoordinates) {
	check(GameDataState.IsValid());

	auto GameDataStatePinned = GameDataState.Pin();
	check(GameDataStatePinned->Matrix->Has(InCoordinates));

	// Toggle the flag on the target cell.
	FMinesweeperCell& Cell = GameDataStatePinned->Matrix->Get(InCoordinates);
	Cell.SetFlagged(!Cell.IsFlagged());
}

void FPlayingLogicState::SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates) {
	check(GameDataState.IsValid());

	auto GameDataStatePinned = GameDataState.Pin();
	check(GameDataStatePinned->Matrix->Has(InCoordinates));

	FMinesweeperCell& Cell = GameDataStatePinned->Matrix->Get(InCoordinates);

	if (Cell.IsRevealed()) {
		// If the cell is already revealed, do nothing.
		return;
	}

	// Reveal the target cell.
	Cell.SetRevealed(true);

	switch (Cell.CellState) {
	case EMinesweeperCellState::Bomb:
		// When interacting with a bomb, transit to game over.
		OwnerStateMachine.Pin()->GoToState<FGameOverLogicState>();
		break;
	case EMinesweeperCellState::Empty:
		// When interacting with an empty space, reveal all its adjacent empty cells recursively (until some bombs are reached in order to stop recursing).
		_revealAdjacents(InCoordinates);
	}
}

void FPlayingLogicState::_revealAdjacents(const FMinesweeperCellCoordinate& InCoordinates) {
	auto Matrix = GameDataState.Pin()->Matrix.ToSharedRef();

	// Count the adjacent bombs to the input coordinates.
	const int NumOfAdjacentBombs = FMinesweeperMatrixNavigator(Matrix).CountAdjacentBombs(InCoordinates);

	// If the input cell has at least one adjacent bomb, stop revealing adjacent empty cells.
	if (NumOfAdjacentBombs > 0 ) {
		return;
	}

	TArray<FIntPoint> AdjacentCellsCoordinates = TMatrixNavigator<FMinesweeperCell>(Matrix).GetAdjacentsTo(InCoordinates);
	for (const auto& AdjacentCellCoordinates : AdjacentCellsCoordinates) {
		if (!Matrix->Has(AdjacentCellCoordinates)) {
			continue;
		}

		// For each adjacent cell that has not been revealed yet and it is empty, reveal it.
		auto& AdjacentCell = Matrix->Get(AdjacentCellCoordinates);
		if (!AdjacentCell.IsRevealed() && AdjacentCell.CellState == EMinesweeperCellState::Empty) {
			AdjacentCell.SetRevealed(true);

			// If the revealed empty cell has no adjacent bombs, reveal also its adjacent empty spaces by recursing on it.
			if (FMinesweeperMatrixNavigator(Matrix).CountAdjacentBombs(AdjacentCellCoordinates) == 0) {
				_revealAdjacents(AdjacentCellCoordinates);
			}
		}
	}
}