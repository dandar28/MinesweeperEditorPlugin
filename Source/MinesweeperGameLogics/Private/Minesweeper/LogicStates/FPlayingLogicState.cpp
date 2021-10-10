#include "Minesweeper/LogicStates/FPlayingLogicState.h"

#include "Minesweeper/EMinesweeperCellState.h"
#include "Minesweeper/FMinesweeperCell.h"
#include "Minesweeper/FMinesweeperMatrixNavigator.h"
#include "Minesweeper/FMinesweeperGameSession.h"
#include "Minesweeper/FMinesweeperGameDataState.h"

#include "Minesweeper/LogicStates/FGameOverLogicState.h"
#include "Minesweeper/LogicStates/FGameWinLogicState.h"

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
		auto MinesweeperMatrixNavigator = FMinesweeperMatrixNavigator(GameDataStatePinned->Matrix.ToSharedRef());
		MinesweeperMatrixNavigator.RevealAdjacentEmptyCellsRecursively(InCoordinates);

		// Win detection.
		if (GameDataStatePinned->IsGameWon()) {
			OwnerStateMachine.Pin()->GoToState<FGameWinLogicState>();
		}
	}
}