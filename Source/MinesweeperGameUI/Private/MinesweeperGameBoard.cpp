#include "MinesweeperGameBoard.h"

void SMinesweeperGameBoard::Construct(const FArguments& InArgs){
	bCanSupportFocus = true;

	_cellsGridPanel =
		SNew(SUniformGridPanel)
		.SlotPadding(FMargin(5.0f));

	ChildSlot
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SImage)
				.ColorAndOpacity(FColor::Black)
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				_cellsGridPanel.ToSharedRef()
			]
		];
}

bool SMinesweeperGameBoard::SupportsKeyboardFocus() const {
	return true;
}

void SMinesweeperGameBoard::PopulateGrid() {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			_cellsGridPanel->AddSlot(i, j)
				[
					SNew(STextBlock)
					.Text(FText::FromString(FString::Printf(TEXT("%d x %d"), i, j)))
				];
		}
	}
}