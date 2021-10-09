#include "Minesweeper/LogicStates/FGameOverLogicState.h"

#include "Minesweeper/FMinesweeperGameSession.h"
#include "Minesweeper/FMinesweeperGameDataState.h"

void FGameOverLogicState::OnEnter() {
	check(GameSession.IsValid());
	check(GameDataState.IsValid());

	GameSession.Pin()->OnGameOver.Broadcast();
	GameDataState.Pin()->RevealAllCells();
}