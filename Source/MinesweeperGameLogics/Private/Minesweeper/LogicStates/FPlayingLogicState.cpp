#include "Minesweeper/LogicStates/FPlayingLogicState.h"

#include "Minesweeper/EMinesweeperCellState.h"
#include "Minesweeper/FMinesweeperCell.h"
#include "Minesweeper/FMinesweeperMatrixNavigator.h"
#include "Minesweeper/FMinesweeperGameSession.h"
#include "Minesweeper/FMinesweeperGameDataState.h"

#include "Minesweeper/LogicStates/FGameOverLogicState.h"
#include "Minesweeper/LogicStates/FGameWinLogicState.h"

#include "Minesweeper/FMinesweeperActions.h"

void FPlayingLogicState::RunAction(TSharedRef<IMinesweeperAction> InAction, const FMinesweeperCellCoordinate& InCoordinates) {
	check(GameSession.IsValid());
	check(GameDataState.IsValid());

	FRegisteredAction PerformedAction;
	PerformedAction.Time = FDateTime::Now();
	PerformedAction.InteractedCell = InCoordinates;
	PerformedAction.Action = InAction;
	PerformedAction.Action->Perform(GameSession.Pin().ToSharedRef(), InCoordinates);

	GameDataState.Pin()->ActionHistory.Actions.Add(PerformedAction);
}

void FPlayingLogicState::OnEnter() {
	check(GameSession.IsValid());
	check(GameDataState.IsValid());

	auto GameSettings = GameSession.Pin()->GetSettings();
	auto GameDataStatePinned = GameDataState.Pin();

	GameDataStatePinned->RebuildMatrix(GameSettings.MatrixBoardSize.X, GameSettings.MatrixBoardSize.Y);
	GameDataStatePinned->ClearAndPlaceRandomMines(GameSettings.NumberOfMines);
	GameDataStatePinned->TickTimer.StartTimer();
}

void FPlayingLogicState::OnExit() {
	check(GameDataState.IsValid());

	GameDataState.Pin()->TickTimer.StopTimer();
}