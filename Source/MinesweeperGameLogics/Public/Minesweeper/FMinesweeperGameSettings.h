#pragma once

#include "CoreMinimal.h"

/**
 * \brief - Structure containing game settings for a minesweeper game session.
 */
struct MINESWEEPERGAMELOGICS_API FMinesweeperGameSettings {
	FIntPoint MatrixBoardSize;
	int NumberOfMines = 0;
};