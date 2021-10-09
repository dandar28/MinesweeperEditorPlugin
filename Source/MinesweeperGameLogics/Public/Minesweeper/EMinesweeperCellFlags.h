#pragma once

#include "CoreMinimal.h"

enum MINESWEEPERGAMELOGICS_API EMinesweeperCellFlags {
	None = 0,
	QuestionMarked = 1<<1,
	Flagged = 1<<2,
	Revealed = 1<<3
};
ENUM_CLASS_FLAGS(EMinesweeperCellFlags)