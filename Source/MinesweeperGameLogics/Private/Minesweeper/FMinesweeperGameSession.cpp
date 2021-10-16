#include "Minesweeper/FMinesweeperGameSession.h"

#include "Minesweeper/IMinesweeperGameLogicState.h"
#include "Minesweeper/FMinesweeperMatrixNavigator.h"

#include "Minesweeper/LogicStates/FIdleLogicState.h"
#include "Minesweeper/LogicStates/FPlayingLogicState.h"
#include "Minesweeper/LogicStates/FGameOverLogicState.h"
#include "Minesweeper/LogicStates/FGameWinLogicState.h"

FMinesweeperGameSession::~FMinesweeperGameSession() {
	if (IsRunning()) {
		Shutdown();
	}
}

void FMinesweeperGameSession::Startup() {
	// Calling Shutdown() on a running session is mandatory before calling Startup() again.
	check(!IsRunning());

	// If already running, better not starting it up again. Shutdown before!
	if (IsRunning()) {
		return;
	}

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

void FMinesweeperGameSession::SetSettings(const FMinesweeperGameSettings& InSettings) {
	_gameSettings = InSettings;
}

FMinesweeperGameSettings FMinesweeperGameSession::GetSettings() const {
	return _gameSettings;
}

void FMinesweeperGameSession::PlayGame() {
	check(IsRunning());
	_gameLogicStateMachine->GoToState<FPlayingLogicState>();
}

void FMinesweeperGameSession::RunAction(TSharedRef<IMinesweeperAction> InAction, const FMinesweeperCellCoordinate& InCoordinates) {
	check(IsRunning());

	auto CurrentLogicState = _gameLogicStateMachine->GetGameLogicStateAs<IMinesweeperGameLogicState>();
	CurrentLogicState->RunAction(InAction, InCoordinates);
}

void FMinesweeperGameSession::MarkOnCell(const FMinesweeperCellCoordinate& InCoordinates) {
	RunAction(FMinesweeperActions::Mark, InCoordinates);
}

void FMinesweeperGameSession::FlagOnCell(const FMinesweeperCellCoordinate& InCoordinates) {
	RunAction(FMinesweeperActions::Flag, InCoordinates);
}

void FMinesweeperGameSession::SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates) {
	RunAction(FMinesweeperActions::Sweep, InCoordinates);
}

TSharedRef<FGameStateMachine> FMinesweeperGameSession::GetLogicStateMachine() const {
	return _gameLogicStateMachine;
}
