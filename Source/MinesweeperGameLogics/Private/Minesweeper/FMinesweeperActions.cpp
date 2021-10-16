#include "Minesweeper/FMinesweeperActions.h"

#include "CellMatrix/ICellMatrix.h"
#include "CellMatrix/TCellMatrix.h"

#include "Minesweeper/FMinesweeperCellCoordinate.h"
#include "Minesweeper/FMinesweeperCell.h"
#include "Minesweeper/FMinesweeperMatrixNavigator.h"
#include "Minesweeper/FMinesweeperGameDataState.h"
#include "Minesweeper/FMinesweeperGameSession.h"

#include "Minesweeper/LogicStates/FGameOverLogicState.h"
#include "Minesweeper/LogicStates/FGameWinLogicState.h"

class FMinesweeperActions::FMinesweeperActionSweep : public IMinesweeperAction {
	void Perform(const TSharedRef<FMinesweeperGameSession> InGameSession, const FMinesweeperCellCoordinate& InCoordinates) override {
		check(InGameSession->IsRunning());

		auto LogicStateMachine = InGameSession->GetLogicStateMachine();

		auto GameDataState = InGameSession->GetGameDataState();
		check(GameDataState->Matrix->Has(InCoordinates));

		FMinesweeperCell& Cell = GameDataState->Matrix->Get(InCoordinates);

		if (Cell.IsRevealed()) {
			// If the cell is already revealed, do nothing.
			return;
		}

		// Reveal the target cell.
		Cell.SetRevealed(true);

		// Unflag and unmark the target cell.
		Cell.SetFlagged(false);
		Cell.SetQuestionMarked(false);

		switch (Cell.CellState) {
		case EMinesweeperCellState::Bomb:
			// When interacting with a bomb, transit to game over.
			LogicStateMachine->GoToState<FGameOverLogicState>();
			break;
		case EMinesweeperCellState::Empty:
			// When interacting with an empty space, reveal all its adjacent empty cells recursively (until some bombs are reached in order to stop recursing).
			auto MinesweeperMatrixNavigator = FMinesweeperMatrixNavigator(GameDataState->Matrix.ToSharedRef());
			MinesweeperMatrixNavigator.RevealAdjacentEmptyCellsRecursively(InCoordinates);

			// Win detection.
			if (GameDataState->IsGameWon()) {
				LogicStateMachine->GoToState<FGameWinLogicState>();
			}
		}
	}
};

class FMinesweeperActions::FMinesweeperActionFlag : public IMinesweeperAction {
	void Perform(const TSharedRef<FMinesweeperGameSession> InGameSession, const FMinesweeperCellCoordinate& InCoordinates) override {
		check(InGameSession->IsRunning());

		auto GameDataState = InGameSession->GetGameDataState();
		check(GameDataState->Matrix->Has(InCoordinates));

		// Obtain a reference to the target cell to flag.
		FMinesweeperCell& Cell = GameDataState->Matrix->Get(InCoordinates);

		// If the target cell is already revealed, we don't want to flag it.
		if (Cell.IsRevealed()) {
			return;
		}

		// Unmark the target cell.
		Cell.SetQuestionMarked(false);

		// Toggle the flag on the target cell.
		Cell.SetFlagged(!Cell.IsFlagged());
	}
};

class FMinesweeperActions::FMinesweeperActionMark : public IMinesweeperAction {
	void Perform(const TSharedRef<FMinesweeperGameSession> InGameSession, const FMinesweeperCellCoordinate& InCoordinates) override {
		check(InGameSession->IsRunning());

		auto GameDataState = InGameSession->GetGameDataState();
		check(GameDataState->Matrix->Has(InCoordinates));

		// Obtain a reference to the target cell to flag.
		FMinesweeperCell& Cell = GameDataState->Matrix->Get(InCoordinates);

		// If the target cell is already revealed, we don't want to mark it.
		if (Cell.IsRevealed()) {
			return;
		}

		// Unflag the target cell.
		Cell.SetFlagged(false);

		// Toggle the question mark on the target cell.
		Cell.SetQuestionMarked(!Cell.IsQuestionMarked());
	}
};

TSharedRef<IMinesweeperAction> FMinesweeperActions::Sweep = MakeShared<FMinesweeperActionSweep>();
TSharedRef<IMinesweeperAction> FMinesweeperActions::Flag = MakeShared<FMinesweeperActionFlag>();
TSharedRef<IMinesweeperAction> FMinesweeperActions::Mark = MakeShared<FMinesweeperActionMark>();