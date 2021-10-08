#pragma once

#include "CoreMinimal.h"

/**
 * \brief - Interface holding a generic game data state
 */
template <class TGameState>
class TGameStateHolder {
public:
	TSharedRef<TGameState> GetGameDataState() const {
		return _gameDataState.ToSharedRef();
	}

protected:
	TSharedPtr<TGameState> _gameDataState;
};

class MINESWEEPERGAMELOGICS_API ILogicState {
public:
	virtual void OnEnter() = 0;
	virtual void OnExit() = 0;
};


class MINESWEEPERGAMELOGICS_API FAbstractLogicState : public ILogicState {
public:
	virtual ~FAbstractLogicState() = default;

	void OnEnter() override {}
	void OnExit() override {}

	TWeakPtr<class FGameStateMachine> OwnerStateMachine;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLogicStateChanged, TSharedRef<FAbstractLogicState>);

/**
 * \brief - Interface holding a generic game logic state machine
 */
class MINESWEEPERGAMELOGICS_API FGameStateMachine : public TSharedFromThis<FGameStateMachine> {
public:
	TSharedRef<ILogicState> GetGameLogicState() const;

	void GoToState(const TSharedRef<FAbstractLogicState>& InNewState);

	template <class TConcreteLogicState>
	TSharedRef<TConcreteLogicState> GetGameLogicStateAs() const {
		return StaticCastSharedRef<TConcreteLogicState>(_logicState.ToSharedRef());
	}

	template <class TConcreteLogicState>
	void GoToState() {
		return GoToState(MakeShared<TConcreteLogicState>());
	}

	FOnLogicStateChanged OnLogicStateChanged;

protected:
	TSharedPtr<ILogicState> _logicState;
};

/**
 * \brief - Interface for hosting a generic game session
 */
class MINESWEEPERGAMELOGICS_API IGameSession {
public:
	virtual void Startup() = 0;
	virtual void Shutdown() = 0;
	virtual bool IsRunning() const = 0;
};

enum MINESWEEPERGAMELOGICS_API EMinesweeperCellState {
	Empty,
	Bomb
};

/**
 * \brief - Structure representing a minesweeper cell
 */
struct MINESWEEPERGAMELOGICS_API FMinesweeperCell {
	EMinesweeperCellState CellState = EMinesweeperCellState::Empty;
	bool bIsFlagged = false;
	bool bIsCovered = true;
};

template <typename CellType>
struct MINESWEEPERGAMELOGICS_API ICellMatrix {
	virtual const CellType& Get(const FIntPoint& InCoordinates) const = 0;
	virtual CellType& Get(const FIntPoint& InCoordinates) = 0;
	virtual bool Has(const FIntPoint& InCoordinates) const = 0;
	virtual FIntPoint GetSize() const = 0;

	virtual int GetNumberOfCells() const {
		const FIntPoint MatrixSize = GetSize();
		return MatrixSize.X * MatrixSize.Y;
	}
};

template <typename CellType>
struct MINESWEEPERGAMELOGICS_API FMatrixNavigator {
	FMatrixNavigator(const TSharedRef<ICellMatrix<CellType>>& matrix) {
		_matrix = matrix;
	}

	TArray<FIntPoint> GetAdjacentsTo(const FIntPoint& InCoordinates, int InSquareUnitDistance = 1) {
		check(_matrix.IsValid());
		auto matrix = _matrix.Pin();
		TArray<FIntPoint> adjacentCells;
		for (int i = -InSquareUnitDistance; i <= InSquareUnitDistance; i++) {
			for (int j = -InSquareUnitDistance; i <= InSquareUnitDistance; j++) {
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
struct MINESWEEPERGAMELOGICS_API TCellMatrix : public ICellMatrix<CellType> {
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

	virtual FIntPoint GetSize() const { return _matrixSize; }

protected:
	TArray<TArray<CellType>> _matrixData;
	FIntPoint _matrixSize;
};

template <int NumRows, int NumCols, typename CellType>
struct MINESWEEPERGAMELOGICS_API TFixedSizeCellMatrix : public ICellMatrix<CellType> {
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
struct MINESWEEPERGAMELOGICS_API FMinesweeperGameDataState {
	FMinesweeperMatrix Matrix;

	void RebuildMatrix(int InWidth, int InHeight);

	void ClearMatrixCells();
	void ClearAndPlaceRandomMines(int InNumberOfMines);
};

using FMinesweeperCellCoordinate = FIntPoint;

/**
 * \brief - Interface handling the game logic state of a minesweeper game session
 */
struct MINESWEEPERGAMELOGICS_API IMinesweeperGameLogicState : public FAbstractLogicState {
	virtual ~IMinesweeperGameLogicState() = default;

	virtual void FlagOnCell(const FMinesweeperCellCoordinate& InCoordinates) = 0;
	virtual void SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates) = 0;

	TWeakPtr<FMinesweeperGameDataState> GameDataState;
};

struct MINESWEEPERGAMELOGICS_API FIdleLogicState : public IMinesweeperGameLogicState {
	virtual ~FIdleLogicState() = default;

	void FlagOnCell(const FMinesweeperCellCoordinate& InCoordinates) override {}
	void SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates) override {}
};

struct MINESWEEPERGAMELOGICS_API FGameOverLogicState : public IMinesweeperGameLogicState {
	virtual ~FGameOverLogicState() = default;

	void FlagOnCell(const FMinesweeperCellCoordinate& InCoordinates) override {}
	void SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates) override {}
};

struct MINESWEEPERGAMELOGICS_API FPlayingLogicState : public IMinesweeperGameLogicState {
	virtual ~FPlayingLogicState() = default;

	void FlagOnCell(const FMinesweeperCellCoordinate& InCoordinates) override;
	void SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates) override;

private:
	void _uncoverAdjacents(const FMinesweeperCellCoordinate& InCoordinates);
};

struct MINESWEEPERGAMELOGICS_API FMinesweeperGameSettings {
	FIntPoint MatrixBoardSize;
	int NumberOfMines = 0;
};

/**
 * \brief - Class hosting a game session of minesweeper
 */
class MINESWEEPERGAMELOGICS_API FMinesweeperGameSession
	: public IGameSession
	, public TGameStateHolder<FMinesweeperGameDataState> {
public:
	virtual ~FMinesweeperGameSession() = default;

	void Startup() override;	
	void Shutdown() override;
	bool IsRunning() const override;

	void PrepareAndStartGame(const FMinesweeperGameSettings& InSettings);

	void FlagOnCell(const FMinesweeperCellCoordinate& InCoordinates);
	void SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates);

private:
	bool _bIsRunning = false;

	TSharedRef<FGameStateMachine> _gameLogicStateMachine = MakeShared<FGameStateMachine>();
};