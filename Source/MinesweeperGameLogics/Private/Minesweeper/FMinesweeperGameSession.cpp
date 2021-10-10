#include "Minesweeper/FMinesweeperGameSession.h"

#include "Minesweeper/IMinesweeperGameLogicState.h"
#include "Minesweeper/FMinesweeperMatrixNavigator.h"

#include "Minesweeper/LogicStates/FIdleLogicState.h"
#include "Minesweeper/LogicStates/FPlayingLogicState.h"
#include "Minesweeper/LogicStates/FGameOverLogicState.h"

FMinesweeperGameSession::~FMinesweeperGameSession() {
	if (IsRunning()) {
		Shutdown();
	}
}

void FMinesweeperGameSession::Startup() {
	_bIsRunning = true;
	_gameDataState = MakeShared<FMinesweeperGameDataState>();

	TWeakPtr<FMinesweeperGameSession> ThisWeakSession = this->AsShared();

	// Add lambda to always inject the game data state to the new logic states when a transition happens
	_gameLogicStateMachine->OnLogicStateChanged.AddLambda([ThisWeakSession](TSharedRef<FAbstractLogicState> InNewState) {
		auto NewMinesweeperGameLogicState = StaticCastSharedRef<IMinesweeperGameLogicState>(InNewState);
		NewMinesweeperGameLogicState->GameDataState = ThisWeakSession.IsValid() ? ThisWeakSession.Pin()->_gameDataState : nullptr;
		NewMinesweeperGameLogicState->GameSession = ThisWeakSession;
	});

	_gameLogicStateMachine->GoToState<FIdleLogicState>();
}

void FMinesweeperGameSession::Shutdown() {
	_gameLogicStateMachine->GoToState<FIdleLogicState>();
	_gameLogicStateMachine->OnLogicStateChanged.Clear();
	_bIsRunning = false;
	_gameDataState.Reset();
}

bool FMinesweeperGameSession::IsRunning() const {
	return _bIsRunning;
}

void FMinesweeperGameSession::PrepareAndStartGame(const FMinesweeperGameSettings& InSettings) {
	check(IsRunning());
	_gameDataState->RebuildMatrix(InSettings.MatrixBoardSize.X, InSettings.MatrixBoardSize.Y);
	_gameDataState->ClearAndPlaceRandomMines(InSettings.NumberOfMines);
	_gameLogicStateMachine->GoToState<FPlayingLogicState>();
}

void FMinesweeperGameSession::FlagOnCell(const FMinesweeperCellCoordinate& InCoordinates) {
	check(IsRunning());

	auto CurrentLogicState = _gameLogicStateMachine->GetGameLogicStateAs<IMinesweeperGameLogicState>();
	CurrentLogicState->FlagOnCell(InCoordinates);
}

void FMinesweeperGameSession::SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates) {
	check(IsRunning());

	auto CurrentLogicState = _gameLogicStateMachine->GetGameLogicStateAs<IMinesweeperGameLogicState>();
	CurrentLogicState->SweepOnCell(InCoordinates);
}