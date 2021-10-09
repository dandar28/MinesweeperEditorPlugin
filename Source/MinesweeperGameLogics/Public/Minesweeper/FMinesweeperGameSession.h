#pragma once

#include "CoreMinimal.h"

#include "CellMatrix/ICellMatrix.h"

#include "GameSession/IGameSession.h"
#include "GameSession/TGameStateHolder.h"

#include "StateMachine/FGameStateMachine.h"

#include "FMinesweeperCellCoordinate.h"
#include "FMinesweeperGameDataState.h"
#include "FMinesweeperGameSettings.h"

DECLARE_MULTICAST_DELEGATE(FOnGameOver);

/**
 * \brief - Class hosting a game session of minesweeper
 */
class MINESWEEPERGAMELOGICS_API FMinesweeperGameSession
	: public IGameSession
	, public TGameStateHolder<FMinesweeperGameDataState>
	, public TSharedFromThis<FMinesweeperGameSession> {
public:
	virtual ~FMinesweeperGameSession();

	void Startup() override;	
	void Shutdown() override;
	bool IsRunning() const override;

	void PrepareAndStartGame(const FMinesweeperGameSettings& InSettings);

	void FlagOnCell(const FMinesweeperCellCoordinate& InCoordinates);
	void SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates);

	FOnGameOver OnGameOver;

private:
	bool _bIsRunning = false;

	TSharedRef<FGameStateMachine> _gameLogicStateMachine = MakeShared<FGameStateMachine>();
};