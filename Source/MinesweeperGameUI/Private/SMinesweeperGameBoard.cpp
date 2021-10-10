#include "SMinesweeperGameBoard.h"

#include "Widgets/Layout/SScaleBox.h"

#include "Minesweeper/FMinesweeperMatrixNavigator.h"

TSharedRef<SVerticalBox> SMinesweeperGameBoard::_makeSettingsArea(const TFunction<void()>& InPlayButtonClicked) {
	const float LateralPadding = 12.0f;
	return
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		.AutoHeight()
		.Padding(LateralPadding, 0.0f, LateralPadding, LateralPadding)
		[
			_makeNumericSettingEntry(TEXT("Width"), _gameSettings->MatrixBoardSize.X, _numericEntryWidth, [this](int InNewValue) { 
				_gameSettings->MatrixBoardSize.X = InNewValue;
			})
		]
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		.AutoHeight()
		.Padding(LateralPadding, 0.0f, LateralPadding, LateralPadding)
		[
			_makeNumericSettingEntry(TEXT("Height"), _gameSettings->MatrixBoardSize.Y, _numericEntryHeight, [this](int InNewValue) { 
				_gameSettings->MatrixBoardSize.Y = InNewValue;
			})
		]
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		.AutoHeight()
		.Padding(LateralPadding, 0.0f, LateralPadding, LateralPadding)
		[
			_makeNumericSettingEntry(TEXT("Number Of Mines"), _gameSettings->NumberOfMines, _numericEntryNumberOfMines, [this](int InNewValue) { 
				_gameSettings->NumberOfMines = InNewValue;
			})
		]
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		.AutoHeight()
		.Padding(LateralPadding, 0.0f, LateralPadding, LateralPadding)
		[
			SNew(SBox)
			.WidthOverride(150)
			.HeightOverride(40)
			.Content()
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("Play!")))
				.OnClicked_Lambda([InPlayButtonClicked]() {
					InPlayButtonClicked();
					return FReply::Handled();
				})
			]
		];
}

TSharedRef<SVerticalBox> SMinesweeperGameBoard::_makeMainGameArea() {
	const float LateralPadding = 12.0f;
	return 
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(LateralPadding, 0.0f, LateralPadding, LateralPadding)
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
		.Padding(LateralPadding, 0.0f, LateralPadding, LateralPadding)
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
		];
}

TSharedRef<SHorizontalBox> SMinesweeperGameBoard::_makeNumericSettingEntry(const FString& InEntryName, const TOptional<int>& InDefaultValue, TSharedPtr<SNumericEntryBox<int>>& OutOwningEntryBox, const TFunction<void(int)>& InOnValueCommitted) {
	TSharedRef<int> Value = MakeShared<int>();
	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(0.5f)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(0.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(InEntryName))
			.Justification(ETextJustify::Type::Left)
			.MinDesiredWidth(60)
		]
		+ SHorizontalBox::Slot()
		.FillWidth(0.5f)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(0.0f)
		[
			SAssignNew(OutOwningEntryBox, SNumericEntryBox<int>)
			.Value(InDefaultValue.Get(0))
			.AllowSpin(true)
			.MinSliderValue(5)
			.MaxSliderValue(90)
			.MinDesiredValueWidth(200)
			.OnValueCommitted_Lambda([InOnValueCommitted, Value](int InNewValue, ETextCommit::Type InCommitType) {
				InOnValueCommitted(InNewValue);
				(*Value) = InNewValue;
			})
			.Value_Lambda([OutOwningEntryBox, Value]() -> TOptional<int32> {
				return (*Value);
			})
		];
}

void SMinesweeperGameBoard::Construct(const FArguments& InArgs){
	bCanSupportFocus = true;

	// Assign the input game session and check its validity.
	_gameSession = InArgs._GameSession;
	check(_gameSession.IsValid());

	// \todo : Handle this differently with predefined difficulty levels' settings.
	if (!_gameSettings.IsSet()) {
		FMinesweeperGameSettings DefaultGameSettings;
		DefaultGameSettings.MatrixBoardSize.X = 9;
		DefaultGameSettings.MatrixBoardSize.Y = 9;
		DefaultGameSettings.NumberOfMines = 10;

		_gameSettings = DefaultGameSettings;
	}

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

	TSharedPtr<SVerticalBox> GameViewBox = _makeMainGameArea();
	TSharedPtr<SVerticalBox> SettingsBox = _makeSettingsArea([this, GameViewBox]() {
		GameViewBox.Get()->SetVisibility(EVisibility::Visible);
		StartGameWithCurrentSettings();
		PopulateGrid();
	});

	ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Fill)
			.AutoHeight()
			.Padding(0.0f, 0.0f, 20.f, 0.0f)
			[
				SettingsBox.ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Fill)
			.FillHeight(1.f)
			.Padding(0.0f, 0.0f, 20.f, 0.0f)
			[
				GameViewBox.ToSharedRef()
			]
		];

	// The game view box is invisible until the Play button is hit.
	GameViewBox->SetVisibility(EVisibility::Hidden);

	// Update the grid view.
	if (_gameSession->IsRunning()) {
		PopulateGrid();
	}
}

bool SMinesweeperGameBoard::SupportsKeyboardFocus() const {
	return true;
}

void SMinesweeperGameBoard::StartGameWithCurrentSettings() {
	check(_gameSession.IsValid());
	check(_gameSettings.IsSet());

	_gameSession->Startup();
	_gameSession->PrepareAndStartGame(_gameSettings.GetValue());
}

void SMinesweeperGameBoard::PopulateGrid() {
	check(_gameSession.IsValid());

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
	check(_gameSession.IsValid());

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
	check(_gameSession.IsValid());

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