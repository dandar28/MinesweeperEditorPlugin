#pragma once

#include "EMinesweeperCellState.h"
#include "EMinesweeperCellFlags.h"

/**
 * \brief - Structure representing a minesweeper cell
 */
struct MINESWEEPERGAMELOGICS_API FMinesweeperCell {
	EMinesweeperCellState CellState = EMinesweeperCellState::Empty;
	EMinesweeperCellFlags Flags = EMinesweeperCellFlags::None;

	bool IsQuestionMarked() const;
	bool IsFlagged() const;
	bool IsRevealed() const;

	void SetQuestionMarked(bool bQuestionMarked);
	void SetFlagged(bool bFlagged);
	void SetRevealed(bool bFlagged);
};