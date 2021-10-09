#pragma once

#include "CoreMinimal.h"

/**
 * \brief - Enum representing the assignable flags related to a minesweeper cell.
 */
enum MINESWEEPERGAMELOGICS_API EMinesweeperCellFlags {
	None = 0,
	QuestionMarked = 1<<1,
	Flagged = 1<<2,
	Revealed = 1<<3
};
ENUM_CLASS_FLAGS(EMinesweeperCellFlags)