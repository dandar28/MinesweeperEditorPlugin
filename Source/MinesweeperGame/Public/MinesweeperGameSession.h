#pragma once

#include "CoreMinimal.h"

/**
 * \brief - Interface holding a generic game data state
 */
template <class TGameState>
class MINESWEEPERGAME_API TGameStateHolder {
public:
	TSharedRef<TGameState> GetGameDataState() const {
		return _gameDataState.ToSharedRef();
	}

protected:
	TSharedPtr<TGameState> _gameDataState;
};

class MINESWEEPERGAME_API ILogicState {
public:
	virtual void OnEnter() = 0;
	virtual void OnExit() = 0;
};


class MINESWEEPERGAME_API FAbstractLogicState : public ILogicState {
public:
	virtual ~FAbstractLogicState() = default;

	void OnEnter() override {}
	void OnExit() override {}

	TWeakPtr<class FGameStateMachine> OwnerStateMachine;
};

/**
 * \brief - Interface holding a generic game logic state machine
 */
class MINESWEEPERGAME_API FGameStateMachine : public TSharedFromThis<FGameStateMachine> {
public:
	TSharedRef<ILogicState> GetGameLogicState() const {
		return _logicState.ToSharedRef();
	}

	template <class TConcreteLogicState>
	TSharedRef<TConcreteLogicState> GetGameLogicStateAs() const {
		return StaticCastSharedRef<TConcreteLogicState>(_logicState.ToSharedRef());
	}

	void GoToState(const TSharedRef<FAbstractLogicState>& newState) {
		if (_logicState.IsValid()) {
			_logicState->OnExit();
		}

		newState->OwnerStateMachine = this->AsShared();
		newState->OnEnter();
	}

	template <class TConcreteLogicState>
	void GoToState() {
		return GoToState(MakeShared<TConcreteLogicState>());
	}

protected:
	TSharedPtr<ILogicState> _logicState;
};

/**
 * \brief - Interface for hosting a generic game session
 */
class MINESWEEPERGAME_API IGameSession {
public:
	virtual void Startup() = 0;
	virtual void Shutdown() = 0;
	virtual bool IsRunning() const = 0;
};

enum MINESWEEPERGAME_API EMinesweeperCellState {
	Empty,
	Bomb
};

/**
 * \brief - Structure representing a minesweeper cell
 */
struct MINESWEEPERGAME_API FMinesweeperCell {
	EMinesweeperCellState CellState;
	bool bIsFlagged = false;
	bool bIsCovered = true;
};

template <typename CellType>
struct MINESWEEPERGAME_API ICellMatrix {
	virtual const CellType& Get(const FIntPoint& InCoordinates) const = 0;
	virtual CellType& Get(const FIntPoint& InCoordinates) = 0;
	virtual bool Has(const FIntPoint& InCoordinates) const = 0;
};

template <typename CellType>
struct MINESWEEPERGAME_API FMatrixNavigator {
	FMatrixNavigator(const TSharedRef<ICellMatrix<CellType>>& matrix) {
		_matrix = matrix;
	}

	TArray<FIntPoint> GetAdjacentsTo(const FIntPoint& InCoordinates, int squareUnitDistance = 1) {
		check(_matrix.IsValid());
		auto matrix = _matrix.Pin();
		TArray<FIntPoint> adjacentCells;
		for (int i = -squareUnitDistance; i <= squareUnitDistance; i++) {
			for (int j = -squareUnitDistance; i <= squareUnitDistance; j++) {
				if (i == 0 && j == 0) {
					continue;
				}
				FIntPoint adjacentCoordinates(InCoordinates + FIntPoint(i, j));
				if (matrix->Has(adjacentCoordinates)) {
					adjacentCells.Add(adjacentCoordinates);
				}
			}
		}
		return adjacentCells;
	}

private:
	TWeakPtr<ICellMatrix<CellType>> _matrix;
};

template <typename CellType>
struct MINESWEEPERGAME_API TCellMatrix : public ICellMatrix<CellType> {
	virtual ~TCellMatrix() = default;

	TCellMatrix(int InWidth, int InHeight) {
		_matrixSize = FIntPoint(InWidth, InHeight);

		_matrixData.SetNum(InWidth);
		for (int i = 0; i < InWidth; i++) {
			_matrixData[i].SetNum(InHeight);
		}
	}

	const CellType& Get(const FIntPoint& InCoordinates) const {
		return _matrixData[InCoordinates.X][InCoordinates.Y];
	}

	CellType& Get(const FIntPoint& InCoordinates) {
		return _matrixData[InCoordinates.X][InCoordinates.Y];
	}

	bool Has(const FIntPoint& InCoordinates) const {
		return InCoordinates.X < _matrixSize.X && InCoordinates.Y < _matrixSize.Y;
	}

protected:
	TArray<TArray<CellType>> _matrixData;
	FIntPoint _matrixSize;
};

template <int NumRows, int NumCols, typename CellType>
struct MINESWEEPERGAME_API TFixedSizeCellMatrix : public ICellMatrix<CellType> {
	const CellType& Get(const FIntPoint& InCoordinates) const {
		return _matrixData[InCoordinates.X][InCoordinates.Y];
	}

	CellType& Get(const FIntPoint& InCoordinates) {
		return _matrixData[InCoordinates.X][InCoordinates.Y];
	}

	bool Has(const FIntPoint& InCoordinates) const {
		return InCoordinates.X < NumRows && InCoordinates.Y < NumCols;
	}

protected:
	CellType _matrixData[NumCols][NumRows];
};

using FMinesweeperMatrix = TSharedPtr<ICellMatrix<FMinesweeperCell>>;

/**
 * \brief - Class holding the game data state of a minesweeper game session
 */
struct MINESWEEPERGAME_API FMinesweeperGameDataState {
	FMinesweeperMatrix Matrix;

	void RebuildMatrix(int InWidth, int InHeight) {
		Matrix = StaticCastSharedRef<ICellMatrix<FMinesweeperCell>>(MakeShared<TCellMatrix<FMinesweeperCell>>(InWidth, InHeight));
	}
};

using FMinesweeperCellCoordinate = FIntPoint;

/**
 * \brief - Interface handling the game logic state of a minesweeper game session
 */
struct MINESWEEPERGAME_API IMinesweeperGameLogicState : public FAbstractLogicState {
	virtual ~IMinesweeperGameLogicState() = default;

	virtual void FlagOnCell(const FMinesweeperCellCoordinate& coordinate) = 0;
	virtual void SweepOnCell(const FMinesweeperCellCoordinate& coordinate) = 0;

	TWeakPtr<FMinesweeperGameDataState> _gameDataState;
};

struct FIdleLogicState : public IMinesweeperGameLogicState {
	virtual ~FIdleLogicState() = default;

	void FlagOnCell(const FMinesweeperCellCoordinate& coordinate) override {}
	void SweepOnCell(const FMinesweeperCellCoordinate& coordinate) override {}
};

struct FGameOverLogicState : public IMinesweeperGameLogicState {
	virtual ~FGameOverLogicState() = default;

	void FlagOnCell(const FMinesweeperCellCoordinate& coordinate) override {}
	void SweepOnCell(const FMinesweeperCellCoordinate& coordinate) override {}
};

struct FPlayingLogicState : public IMinesweeperGameLogicState {
	virtual ~FPlayingLogicState() = default;

	void FlagOnCell(const FMinesweeperCellCoordinate& coordinate) override {
		check(_gameDataState.IsValid());

		auto gameDataState = _gameDataState.Pin();
		check(gameDataState->Matrix->Has(coordinate));
		FMinesweeperCell& cell = gameDataState->Matrix->Get(coordinate);
		cell.bIsFlagged = !cell.bIsFlagged;
	}

	void SweepOnCell(const FMinesweeperCellCoordinate& coordinate) override {
		check(_gameDataState.IsValid());

		auto gameDataState = _gameDataState.Pin();
		check(gameDataState->Matrix->Has(coordinate));
		FMinesweeperCell& cell = gameDataState->Matrix->Get(coordinate);
		switch (cell.CellState) {
		case EMinesweeperCellState::Bomb:
			OwnerStateMachine.Pin()->GoToState<FGameOverLogicState>();
			break;
		case EMinesweeperCellState::Empty:
			_gameDataState.Pin()->Matrix->Get(coordinate).bIsCovered = false;
			_uncoverAdjacents(coordinate);
		}
	}

private:
	void _uncoverAdjacents(const FMinesweeperCellCoordinate& coordinate) {
		auto matrix = _gameDataState.Pin()->Matrix.ToSharedRef();

		TArray<FIntPoint> adjacentCellsCoordinates = FMatrixNavigator<FMinesweeperCell>(matrix).GetAdjacentsTo(coordinate);
		for (const auto& adjacentCellCoordinates : adjacentCellsCoordinates) {
			auto& adjacentCell = matrix->Get(adjacentCellCoordinates);
			if (adjacentCell.bIsCovered && adjacentCell.CellState == EMinesweeperCellState::Empty) {
				adjacentCell.bIsCovered = false;
				_uncoverAdjacents(adjacentCellCoordinates);
			}
		}
	}
};

/**
 * \brief - Class hosting a game session of minesweeper
 */
class MINESWEEPERGAME_API FMinesweeperGameSession : public IGameSession, public TGameStateHolder<FMinesweeperGameDataState> {
	virtual ~FMinesweeperGameSession() = default;

	void Startup() override {
		_bIsRunning = true;
		_gameDataState = MakeShared<FMinesweeperGameDataState>();
		_gameLogicStateMachine.GoToState<FPlayingLogicState>();
	}
	
	void Shutdown() override {
		_gameLogicStateMachine.GoToState<FIdleLogicState>();
		_bIsRunning = false;
		_gameDataState.Reset();
	}

	bool IsRunning() const override { return _bIsRunning; }

	void FlagOnCell(const FMinesweeperCellCoordinate& coordinate) {
		check(IsRunning());
		_gameLogicStateMachine.GetGameLogicStateAs<IMinesweeperGameLogicState>()->FlagOnCell(coordinate);
	}

	void SweepOnCell(const FMinesweeperCellCoordinate& coordinate) {
		check(IsRunning());
		_gameLogicStateMachine.GetGameLogicStateAs<IMinesweeperGameLogicState>()->SweepOnCell(coordinate);
	}

private:
	bool _bIsRunning = false;

	FGameStateMachine _gameLogicStateMachine;
};