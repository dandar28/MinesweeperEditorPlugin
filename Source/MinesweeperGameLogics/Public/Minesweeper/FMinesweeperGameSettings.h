#pragma once

#include "CoreMinimal.h"

/**
 * \brief - Structure containing game settings for a minesweeper game session.
 */
struct MINESWEEPERGAMELOGICS_API FMinesweeperGameSettings {
	/** \brief - Bidimensional size of the game board matrix. */
	FIntPoint MatrixBoardSize;

	/** \brief - Number of mines to be spawned for the game. */
	int NumberOfMines = 0;
};