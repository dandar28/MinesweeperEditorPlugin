#include "MinesweeperGameBoard.h"

void SMinesweeperGameBoard::Construct(const FArguments& InArgs){
	bCanSupportFocus = true;

	_gameSession = InArgs._GameSession;
	check(_gameSession.IsValid());

	_cellsGridPanel =
		SNew(SUniformGridPanel)
		.SlotPadding(FMargin(5.0f));

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
			.AutoHeight()
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
					_cellsGridPanel.ToSharedRef()
				]
			]
		];

	PopulateGrid();
}

bool SMinesweeperGameBoard::SupportsKeyboardFocus() const {
	return true;
}

void SMinesweeperGameBoard::PopulateGrid() {
	const FIntPoint BoardMatrixSize = _gameSession->GetGameDataState()->Matrix->GetSize();

	_cellsGridPanel->ClearChildren();

	const auto Matrix = _gameSession->GetGameDataState()->Matrix;

	for (int ColumnIndex = 0; ColumnIndex < BoardMatrixSize.X; ColumnIndex++) {
		for (int RowIndex = 0; RowIndex < BoardMatrixSize.Y; RowIndex++) {
			FLinearColor CurrentCellColor = FLinearColor::White;
			FString CurrentCellText = "?";
			const FMinesweeperCellCoordinate CurrentCellCoordinates = FMinesweeperCellCoordinate(ColumnIndex, RowIndex);
			const FMinesweeperCell CurrentCell = Matrix->Get(CurrentCellCoordinates);
			int CountOfAdjacentBombs = 0;

			if (!CurrentCell.bIsCovered) {
				switch (CurrentCell.CellState) {
				case EMinesweeperCellState::Empty:
					CurrentCellText = "";

					CountOfAdjacentBombs = FMinesweeperMatrixNavigator(Matrix.ToSharedRef()).CountAdjacentBombs(CurrentCellCoordinates);
					if (CountOfAdjacentBombs > 0) {
						CurrentCellText = FString::FromInt(CountOfAdjacentBombs);
					}

					switch (CountOfAdjacentBombs) {
					case 0:
						CurrentCellColor = FLinearColor::Transparent;
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
			} else if (CurrentCell.bIsFlagged) {
				CurrentCellColor = FLinearColor::Yellow;
				CurrentCellText = "FLAG";
			}

			const auto CellCoordinates = FMinesweeperCellCoordinate(ColumnIndex, RowIndex);
			_cellsGridPanel->AddSlot(ColumnIndex, RowIndex)
				[
					SNew(SButton)
					.Text(FText::FromString(CurrentCellText))
					.ButtonColorAndOpacity(CurrentCellColor)
					.OnClicked_Lambda([this, CellCoordinates]() {
						RunAction(CellCoordinates);
						return FReply::Handled();
					})
				];
		}
	}
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
