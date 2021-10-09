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
 * \brief - Class hosting a game session of minesweeper.
 */
class MINESWEEPERGAMELOGICS_API FMinesweeperGameSession
	: public IGameSession
	, public TGameStateHolder<FMinesweeperGameDataState>
	, public TSharedFromThis<FMinesweeperGameSession> {
public:
	virtual ~FMinesweeperGameSession();

	/**
	 * \brief - Prepare game data state, initialize state machine's to idle and add bindings.
	 * \note - This method needs to be called before anything else.
	 */
	void Startup() override;

	/**
	 * \brief - Destroy game data state, reset state machine's state to idle and clear bindings.
	 * \note - This method can be called in the end to clear everything before this instance dies
	 *			but you can also forget to call it since it is called from the destructor anyway.
	 */
	void Shutdown() override;

	/**
	 * \return True if this game session is running: after startup and before shutdown.
	 */
	bool IsRunning() const override;

	/**
	 * \brief - Prepare this game session through some settings after starting up the session.
	 * \param[in] InSettings - Settings to prepare this game session for.
	 */
	void PrepareAndStartGame(const FMinesweeperGameSettings& InSettings);

	/**
	 * \brief - Put a flag on certain cell coordinates.
	 * \param[in] InCoordinates - Target cell coordinates.
	 */
	void FlagOnCell(const FMinesweeperCellCoordinate& InCoordinates);

	/**
	 * \brief - Try to sweep on certain cell coordinates.
	 * \param[in] InCoordinates - Target cell coordinates.
	 */
	void SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates);

	/**
	 * \brief - Event delegate that is triggered whenever the player hits the game over on this game session.
	 */
	FOnGameOver OnGameOver;

private:
	/**
	 * \brief - Boolean true when this game session is running after being started up and before being shut down.
	 */
	bool _bIsRunning = false;

	/**
	 * \brief - Instance of the Finite State Machine of this game's logic states.
	 */
	TSharedRef<FGameStateMachine> _gameLogicStateMachine = MakeShared<FGameStateMachine>();
};