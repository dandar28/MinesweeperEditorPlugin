#include "MinesweeperGameSession.h"

TSharedRef<ILogicState> FGameStateMachine::GetGameLogicState() const {
	return _logicState.ToSharedRef();
}

void FGameStateMachine::GoToState(const TSharedRef<FAbstractLogicState>& InNewState) {
	if (_logicState.IsValid()) {
		_logicState->OnExit();
	}

	_logicState = InNewState;
	InNewState->OwnerStateMachine = this->AsShared();
	InNewState->OnEnter();
}

void FMinesweeperGameDataState::RebuildMatrix(int InWidth, int InHeight) {
	Matrix = StaticCastSharedRef<ICellMatrix<FMinesweeperCell>>(MakeShared<TCellMatrix<FMinesweeperCell>>(InWidth, InHeight));

	ClearMatrixCells();
}

void FMinesweeperGameDataState::ClearMatrixCells() {
	check(Matrix.IsValid());

	// Fill all cells with default empty mine cells
	for (int CurrentColumn = 0; CurrentColumn < Matrix->GetSize().X; CurrentColumn++) {
		for (int CurrentRow = 0; CurrentRow < Matrix->GetSize().Y; CurrentRow++) {
			Matrix->Get(FMinesweeperCellCoordinate(CurrentColumn, CurrentRow)) = FMinesweeperCell();
		}
	}
}

void FMinesweeperGameDataState::ClearAndPlaceRandomMines(int InNumberOfMines) {
	check(Matrix.IsValid());

	const int NumberOfCells = Matrix->GetNumberOfCells();
	check(NumberOfCells > 0);
	check(InNumberOfMines <= NumberOfCells);

	TArray<FMinesweeperCellCoordinate> FreeCells;
	for (int CurrentColumn = 0; CurrentColumn < Matrix->GetSize().X; CurrentColumn++) {
		for (int CurrentRow = 0; CurrentRow < Matrix->GetSize().Y; CurrentRow++) {
			FreeCells.Add(FMinesweeperCellCoordinate(CurrentColumn, CurrentRow));
		}
	}

	for (int MineIndex = 0; MineIndex < InNumberOfMines; MineIndex++) {
		FMinesweeperCellCoordinate PickedFreeCell = FreeCells[FMath::RandRange(0, FreeCells.Num() - 1)];
		Matrix->Get(PickedFreeCell).CellState = EMinesweeperCellState::Bomb;
	}
}

void FPlayingLogicState::FlagOnCell(const FMinesweeperCellCoordinate& InCoordinates) {
	check(_gameDataState.IsValid());

	auto gameDataState = _gameDataState.Pin();
	check(gameDataState->Matrix->Has(InCoordinates));
	FMinesweeperCell& cell = gameDataState->Matrix->Get(InCoordinates);
	cell.bIsFlagged = !cell.bIsFlagged;
}

void FPlayingLogicState::SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates) {
	check(_gameDataState.IsValid());

	auto gameDataState = _gameDataState.Pin();
	check(gameDataState->Matrix->Has(InCoordinates));
	FMinesweeperCell& cell = gameDataState->Matrix->Get(InCoordinates);
	switch (cell.CellState) {
	case EMinesweeperCellState::Bomb:
		OwnerStateMachine.Pin()->GoToState<FGameOverLogicState>();
		break;
	case EMinesweeperCellState::Empty:
		_gameDataState.Pin()->Matrix->Get(InCoordinates).bIsCovered = false;
		_uncoverAdjacents(InCoordinates);
	}
}

void FPlayingLogicState::_uncoverAdjacents(const FMinesweeperCellCoordinate& InCoordinates) {
	auto matrix = _gameDataState.Pin()->Matrix.ToSharedRef();

	TArray<FIntPoint> adjacentCellsCoordinates = FMatrixNavigator<FMinesweeperCell>(matrix).GetAdjacentsTo(InCoordinates);
	for (const auto& adjacentCellCoordinates : adjacentCellsCoordinates) {
		auto& adjacentCell = matrix->Get(adjacentCellCoordinates);
		if (adjacentCell.bIsCovered && adjacentCell.CellState == EMinesweeperCellState::Empty) {
			adjacentCell.bIsCovered = false;
			_uncoverAdjacents(adjacentCellCoordinates);
		}
	}
}

void FMinesweeperGameSession::Startup() {
	_bIsRunning = true;
	_gameDataState = MakeShared<FMinesweeperGameDataState>();
	_gameLogicStateMachine->GoToState<FIdleLogicState>();
}

void FMinesweeperGameSession::Shutdown() {
	_gameLogicStateMachine->GoToState<FIdleLogicState>();
	_bIsRunning = false;
	_gameDataState.Reset();
}

bool FMinesweeperGameSession::IsRunning() const { return _bIsRunning; }

void FMinesweeperGameSession::PrepareAndStartGame(const FMinesweeperGameSettings& InSettings) {
	check(IsRunning());
	_gameDataState->RebuildMatrix(InSettings.MatrixBoardSize.X, InSettings.MatrixBoardSize.Y);
	_gameDataState->ClearAndPlaceRandomMines(InSettings.NumberOfMines);
	_gameLogicStateMachine->GoToState<FPlayingLogicState>();
}

void FMinesweeperGameSession::FlagOnCell(const FMinesweeperCellCoordinate& InCoordinates) {
	check(IsRunning());
	_gameLogicStateMachine->GetGameLogicStateAs<IMinesweeperGameLogicState>()->FlagOnCell(InCoordinates);
}

void FMinesweeperGameSession::SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates) {
	check(IsRunning());
	_gameLogicStateMachine->GetGameLogicStateAs<IMinesweeperGameLogicState>()->SweepOnCell(InCoordinates);
}
