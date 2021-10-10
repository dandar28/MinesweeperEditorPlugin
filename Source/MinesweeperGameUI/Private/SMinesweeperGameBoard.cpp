#include "SMinesweeperGameBoard.h"

#include "Widgets/Layout/SScaleBox.h"

#include "Minesweeper/FMinesweeperMatrixNavigator.h"

void SMinesweeperGameBoard::Construct(const FArguments& InArgs){
	bCanSupportFocus = true;

	// Assign the input game session and check its validity.
	_gameSession = InArgs._GameSession;
	check(_gameSession.IsValid());

	// When the player loses the game, show a popup and update the view.
	_gameSession->OnGameOver.AddLambda([this]() {
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("You Lost!"));

		PopulateGrid();
	});

	// Create a uniform grid panel to host the cells.
	_cellsGridPanel =
		SNew(SUniformGridPanel)
		.SlotPadding(FMargin(2.0f))
		.MinDesiredSlotWidth(40)
		.MinDesiredSlotHeight(40);

	ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()			
			.AutoHeight()
			.Padding(12.0f, 0.0f, 12.0f, 12.0f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(0.0f)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Width")))
					.Justification(ETextJustify::Type::Left)
					.MinDesiredWidth(60)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(0.0f)
				[
					SNew(SEditableTextBox)
					.Text(FText::FromString(TEXT("0")))
					.Justification(ETextJustify::Type::Left)
					.MinDesiredWidth(60)
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(12.0f, 0.0f, 12.0f, 12.0f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(0.0f)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Height")))
					.Justification(ETextJustify::Type::Left)
					.MinDesiredWidth(60)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(0.0f)
				[
					SNew(SEditableTextBox)
					.Text(FText::FromString(TEXT("0")))
					.Justification(ETextJustify::Type::Left)
					.MinDesiredWidth(60)
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(12.0f, 0.0f, 12.0f, 12.0f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(0.0f)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Number Of Mines")))
					.Justification(ETextJustify::Type::Left)
					.MinDesiredWidth(60)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(0.0f)
				[
					SNew(SEditableTextBox)
					.Text(FText::FromString(TEXT("0")))
					.Justification(ETextJustify::Type::Left)
					.MinDesiredWidth(60)
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(12.0f, 0.0f, 12.0f, 12.0f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(0.0f)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Current Action")))
					.Justification(ETextJustify::Type::Left)
					.MinDesiredWidth(60)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(0.0f)
				[
					SNew(SBox)
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Fill)
					.HeightOverride(21)
					[
						SNew(SComboBox<TSharedPtr<FText>>)
						.OptionsSource(&_actionComboOptions)
						.IsEnabled(true)
						.OnGenerateWidget_Lambda([](TSharedPtr<FText> Item)
						{
							return SNew(STextBlock).Text(*Item);
						})
						.Content()
						[
							SNew(STextBlock)
							.Text(this, &SMinesweeperGameBoard::GetSelectedActionOption)
						]
						.OnSelectionChanged(this, &SMinesweeperGameBoard::OnSelectedActionChanged)
					]
				]
			]
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Fill)
			.FillHeight(1.f)
			.Padding(12.0f, 0.0f, 12.0f, 12.0f)
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
					SNew(SScaleBox)
					.StretchDirection(EStretchDirection::Both)
					.Stretch(EStretch::ScaleToFit)
					.Content()
					[
						_cellsGridPanel.ToSharedRef()
					]
				]
			]
		];

	// Update the grid view.
	PopulateGrid();
}

bool SMinesweeperGameBoard::SupportsKeyboardFocus() const {
	return true;
}

void SMinesweeperGameBoard::PopulateGrid() {
	const auto Matrix = _gameSession->GetGameDataState()->Matrix;
	const FIntPoint BoardMatrixSize = Matrix->GetSize();

	// Clear the children of the cells grid panel in order to readd all children from zero.
	_cellsGridPanel->ClearChildren();

	// For each cell, determine the appearence of the cell and add the element as child of the cells grid panel.
	FMinesweeperMatrixNavigator(Matrix.ToSharedRef()).ForeachCell([this](const FMinesweeperCellCoordinate& InCoordinates, FMinesweeperCell& InRefCell) {
		int ColumnIndex = InCoordinates.X;
		int RowIndex = InCoordinates.Y;
		_cellsGridPanel->AddSlot(ColumnIndex, RowIndex)
			[
				_makeWidgetForCell(InCoordinates, InRefCell)
			];
	});
}

void SMinesweeperGameBoard::RunAction(const FMinesweeperCellCoordinate& InCoordinates) {
	_actionFunctions[_selectedActionIndex](_gameSession, InCoordinates);

	PopulateGrid();
}

void SMinesweeperGameBoard::OnSelectedActionChanged(TSharedPtr<FText> InNewItem, ESelectInfo::Type InSelectType) {
	_selectedActionIndex = _actionComboOptions.IndexOfByPredicate([InNewItem](TSharedPtr<FText> InElement) {
		return InElement->ToString() == InNewItem->ToString();
	});
}

FText SMinesweeperGameBoard::GetSelectedActionOption() const { 
	return *_actionComboOptions[_selectedActionIndex].Get();
}

TSharedRef<SWidget> SMinesweeperGameBoard::_makeWidgetForCell(const FMinesweeperCellCoordinate& InCellCoordinates, const FMinesweeperCell& InCell) {
	const auto Matrix = _gameSession->GetGameDataState()->Matrix;

	FLinearColor CurrentCellColor = FLinearColor::White;
	FString CurrentCellText = "?";

	int CountOfAdjacentBombs = 0;

	if (InCell.IsRevealed()) {
		switch (InCell.CellState) {
		case EMinesweeperCellState::Empty:
			CurrentCellText = "";

			CountOfAdjacentBombs = FMinesweeperMatrixNavigator(Matrix.ToSharedRef()).CountAdjacentBombs(InCellCoordinates);
			if (CountOfAdjacentBombs > 0) {
				CurrentCellText = FString::FromInt(CountOfAdjacentBombs);
			}

			switch (CountOfAdjacentBombs) {
			case 0:
				CurrentCellColor = FLinearColor(0.4, 0.4, 0.4);
				break;
			case 1:
				CurrentCellColor = FLinearColor::Blue;
				break;
			case 2:
				CurrentCellColor = FLinearColor::Green;
				break;
			case 3:
			default:
				CurrentCellColor = FLinearColor::Red;
				break;
			}
			break;
		case EMinesweeperCellState::Bomb:
			CurrentCellText = "BOMB";
			break;
		}
	}
	else if (InCell.IsFlagged()) {
		CurrentCellColor = FLinearColor::Yellow;
		CurrentCellText = "FLAG";
	}

	return 
		SNew(SButton)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Text(FText::FromString(CurrentCellText))
		.ButtonColorAndOpacity(CurrentCellColor)
		.OnClicked_Lambda([this, InCellCoordinates]() {
			RunAction(InCellCoordinates);
			return FReply::Handled();
		});
}