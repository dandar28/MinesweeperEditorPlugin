#include "Minesweeper/FMinesweeperCell.h"

bool FMinesweeperCell::IsQuestionMarked() const { return EnumHasAnyFlags(Flags, EMinesweeperCellFlags::QuestionMarked); }
bool FMinesweeperCell::IsFlagged() const { return EnumHasAnyFlags(Flags, EMinesweeperCellFlags::Flagged); }
bool FMinesweeperCell::IsRevealed() const { return EnumHasAnyFlags(Flags, EMinesweeperCellFlags::Revealed); }

void FMinesweeperCell::SetQuestionMarked(bool bQuestionMarked) { Flags |= EMinesweeperCellFlags::QuestionMarked; }
void FMinesweeperCell::SetFlagged(bool bFlagged) { Flags |= EMinesweeperCellFlags::Flagged; }
void FMinesweeperCell::SetRevealed(bool bFlagged) { Flags |= EMinesweeperCellFlags::Revealed; }