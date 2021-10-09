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
	OnLogicStateChanged.Broadcast(InNewState);

	InNewState->OnEnter();
}

void FMinesweeperGameDataState::RebuildMatrix(int InWidth, int InHeight) {
	Matrix = StaticCastSharedRef<ICellMatrix<FMinesweeperCell>>(MakeShared<TCellMatrix<FMinesweeperCell>>(InWidth, InHeight));

	ClearMatrixCells();
}

void FMinesweeperGameDataState::ForeachCell(const TFunction<void(FMinesweeperCell&)>& InPredicate) {
	check(!!InPredicate);
	check(Matrix.IsValid());

	for (int CurrentColumn = 0; CurrentColumn < Matrix->GetSize().X; CurrentColumn++) {
		for (int CurrentRow = 0; CurrentRow < Matrix->GetSize().Y; CurrentRow++) {
			FMinesweeperCell& CurrentCell = Matrix->Get(FMinesweeperCellCoordinate(CurrentColumn, CurrentRow));
			InPredicate(CurrentCell);
		}
	}
}

void FMinesweeperGameDataState::RevealAllCells() {
	ForeachCell([](FMinesweeperCell& InRefCurrentCell) {
		InRefCurrentCell.SetRevealed(true);
	});
}

void FMinesweeperGameDataState::ClearMatrixCells() {
	ForeachCell([](FMinesweeperCell& InRefCurrentCell) {
		InRefCurrentCell = FMinesweeperCell();
	});
}

void FMinesweeperGameDataState::ClearAndPlaceRandomMines(int InNumberOfMines) {
	check(Matrix.IsValid());

	ClearMatrixCells();

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

void FGameOverLogicState::OnEnter() {
	check(GameSession.IsValid());
	check(GameDataState.IsValid());

	GameSession.Pin()->OnGameOver.Broadcast();
	GameDataState.Pin()->RevealAllCells();
}

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

	TArray<FIntPoint> AdjacentCellsCoordinates = FMatrixNavigator<FMinesweeperCell>(Matrix).GetAdjacentsTo(InCoordinates);
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

bool FMinesweeperCell::IsQuestionMarked() const { return EnumHasAnyFlags(Flags, EMinesweeperCellFlags::QuestionMarked); }
bool FMinesweeperCell::IsFlagged() const { return EnumHasAnyFlags(Flags, EMinesweeperCellFlags::Flagged); }
bool FMinesweeperCell::IsRevealed() const { return EnumHasAnyFlags(Flags, EMinesweeperCellFlags::Revealed); }

void FMinesweeperCell::SetQuestionMarked(bool bQuestionMarked) { Flags |= EMinesweeperCellFlags::QuestionMarked; }
void FMinesweeperCell::SetFlagged(bool bFlagged) { Flags |= EMinesweeperCellFlags::Flagged; }
void FMinesweeperCell::SetRevealed(bool bFlagged) { Flags |= EMinesweeperCellFlags::Revealed; }
