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
	FMinesweeperCell& Cell = GameDataStatePinned->Matrix->Get(InCoordinates);
	Cell.SetFlagged(!Cell.IsFlagged());
}

void FPlayingLogicState::SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates) {
	check(GameDataState.IsValid());

	auto GameDataStatePinned = GameDataState.Pin();
	check(GameDataStatePinned->Matrix->Has(InCoordinates));

	FMinesweeperCell& Cell = GameDataStatePinned->Matrix->Get(InCoordinates);

	if (Cell.IsRevealed()) {
		return;
	}

	Cell.SetRevealed(true);

	switch (Cell.CellState) {
	case EMinesweeperCellState::Bomb:
		OwnerStateMachine.Pin()->GoToState<FGameOverLogicState>();
		break;
	case EMinesweeperCellState::Empty:
		_revealAdjacents(InCoordinates);
	}
}

void FPlayingLogicState::_revealAdjacents(const FMinesweeperCellCoordinate& InCoordinates) {
	auto Matrix = GameDataState.Pin()->Matrix.ToSharedRef();

	const int NumOfAdjacentBombs = FMinesweeperMatrixNavigator(Matrix).CountAdjacentBombs(InCoordinates);
	if (NumOfAdjacentBombs > 0 ) {
		return;
	}

	TArray<FIntPoint> AdjacentCellsCoordinates = TMatrixNavigator<FMinesweeperCell>(Matrix).GetAdjacentsTo(InCoordinates);
	for (const auto& AdjacentCellCoordinates : AdjacentCellsCoordinates) {
		if (!Matrix->Has(AdjacentCellCoordinates)) {
			continue;
		}
		auto& AdjacentCell = Matrix->Get(AdjacentCellCoordinates);
		if (!AdjacentCell.IsRevealed() && AdjacentCell.CellState == EMinesweeperCellState::Empty) {
			AdjacentCell.SetRevealed(true);

			if (FMinesweeperMatrixNavigator(Matrix).CountAdjacentBombs(AdjacentCellCoordinates) == 0) {
				_revealAdjacents(AdjacentCellCoordinates);
			}
		}
	}
}