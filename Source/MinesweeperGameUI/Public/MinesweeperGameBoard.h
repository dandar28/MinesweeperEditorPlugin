#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

class MINESWEEPERGAMEUI_API SMinesweeperGameBoard : public SCompoundWidget {
public:

	SLATE_BEGIN_ARGS(SMinesweeperGameBoard) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	bool SupportsKeyboardFocus() const override;

	void PopulateGrid();

private:

	TSharedPtr<SUniformGridPanel> _cellsGridPanel;
};