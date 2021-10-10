#include "Minesweeper/LogicStates/FGameWinLogicState.h"

#include "Minesweeper/FMinesweeperGameSession.h"
#include "Minesweeper/FMinesweeperGameDataState.h"

void FGameWinLogicState::OnEnter() {
	check(GameSession.IsValid());
	check(GameDataState.IsValid());

	GameSession.Pin()->OnGameWin.Broadcast();
	GameDataState.Pin()->RevealAllCells();
}