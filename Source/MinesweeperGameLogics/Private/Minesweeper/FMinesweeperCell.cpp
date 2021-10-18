#include "Minesweeper/FMinesweeperCell.h"

bool FMinesweeperCell::IsQuestionMarked() const { return EnumHasAnyFlags(Flags, EMinesweeperCellFlags::QuestionMarked); }
bool FMinesweeperCell::IsFlagged() const { return EnumHasAnyFlags(Flags, EMinesweeperCellFlags::Flagged); }
bool FMinesweeperCell::IsRevealed() const { return EnumHasAnyFlags(Flags, EMinesweeperCellFlags::Revealed); }

void FMinesweeperCell::SetQuestionMarked(bool bQuestionMarked) {
	if (bQuestionMarked) {
		Flags = EMinesweeperCellFlags::QuestionMarked;
	} else {
		Flags &= ~EMinesweeperCellFlags::QuestionMarked;
	}
}

void FMinesweeperCell::SetFlagged(bool bFlagged) {
	if (bFlagged) {
		Flags = EMinesweeperCellFlags::Flagged;
	} else {
		Flags &= ~EMinesweeperCellFlags::Flagged;
	}
}

void FMinesweeperCell::SetRevealed(bool bRevealed) {
	if (bRevealed) {
		Flags = EMinesweeperCellFlags::Revealed;
	} else {
		Flags &= ~EMinesweeperCellFlags::Revealed;
	}
}