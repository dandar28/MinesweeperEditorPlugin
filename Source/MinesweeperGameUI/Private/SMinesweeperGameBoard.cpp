#include "SMinesweeperGameBoard.h"

#include "Widgets/Layout/SScaleBox.h"

#include "Minesweeper/FMinesweeperMatrixNavigator.h"

TSharedRef<SVerticalBox> SMinesweeperGameBoard::_makeSettingsArea(const TFunction<void()>& InPlayButtonClicked) {
	const auto BombsCountValueClamper = [this](int InValue) -> int {
		const auto MatrixSize = _gameSettings->MatrixBoardSize;
		const int MatrixBoardArea = MatrixSize.X * MatrixSize.Y;
		return FMath::Clamp<int>(InValue, 1, MatrixBoardArea);
	};

	const float LateralPadding = 12.0f;
	return
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		.AutoHeight()
		.Padding(LateralPadding, 0.0f, LateralPadding, LateralPadding)
		[
			_makeNumericSettingEntry(
				TNumericSettingEntry<int>()
				.SetEntryName(TEXT("Width"))
				.SetValueGetter([this]() {
					return _gameSettings->MatrixBoardSize.X;
				})
				.SetValueSetter([this](int InNewValue) {
					_gameSettings->MatrixBoardSize.X = InNewValue;
				})
				.SetMinValue(1)
				.SetMaxValue(80),
				_numericEntryWidth
			)
		]
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		.AutoHeight()
		.Padding(LateralPadding, 0.0f, LateralPadding, LateralPadding)
		[
			_makeNumericSettingEntry(
				TNumericSettingEntry<int>()
				.SetEntryName(TEXT("Height"))
				.SetValueGetter([this]() {
					return _gameSettings->MatrixBoardSize.Y;
				})
				.SetValueSetter([this](int InNewValue) {
					_gameSettings->MatrixBoardSize.Y = InNewValue;
				})
				.SetMinValue(1)
				.SetMaxValue(80), 
				_numericEntryHeight
			)
		]
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		.AutoHeight()
		.Padding(LateralPadding, 0.0f, LateralPadding, LateralPadding)
		[
			_makeNumericSettingEntry(
				TNumericSettingEntry<int>()
				.SetEntryName(TEXT("Number Of Mines"))
				.SetValueGetter([this, BombsCountValueClamper]() {
					return BombsCountValueClamper(_gameSettings->NumberOfMines);
				})
				.SetValueSetter([this, BombsCountValueClamper](int InNewValue) {
					_gameSettings->NumberOfMines = BombsCountValueClamper(InNewValue);
				})
				.SetMinValue(1)
				.SetMaxValue(999), 
				_numericEntryNumberOfMines
			)
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
				.Text_Lambda([this]() -> FText {
					const FString PlayResetButtonText = _gameSession->IsRunning() ? TEXT("Stop!") : TEXT("Play!");
					return FText::FromString(PlayResetButtonText);
				})
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

TSharedRef<SHorizontalBox> SMinesweeperGameBoard::_makeNumericSettingEntry(const TNumericSettingEntry<int>& InNumericSettingEntry, TSharedPtr<SNumericEntryBox<int>>& OutOwningEntryBox) {
	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(0.5f)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(0.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(InNumericSettingEntry.EntryName))
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
			.Value(InNumericSettingEntry.ValueGetter().Get(0))
			.MinValue(InNumericSettingEntry.MinValue)
			.MaxValue(InNumericSettingEntry.MaxValue)
			.MinDesiredValueWidth(200)
			.OnValueCommitted_Lambda([InNumericSettingEntry](int InNewValue, ETextCommit::Type InCommitType) {
				InNumericSettingEntry.ValueSetter(InNumericSettingEntry.Clamp(InNewValue));
			})
			.Value_Lambda([InNumericSettingEntry]() -> TOptional<int32> {
				return InNumericSettingEntry.Clamp(InNumericSettingEntry.ValueGetter().Get(0));
			})
		];
}

void SMinesweeperGameBoard::Construct(const FArguments& InArgs){
	bCanSupportFocus = true;

	// Assign the input game session and check its validity.
	_gameSession = InArgs._GameSession;
	check(_gameSession.IsValid());

	// Populate all difficulty levels' settings
	{
		_difficultyLevelsSettings.Add(EDifficultyLevel::Beginner, ([]() -> FMinesweeperGameSettings {
			FMinesweeperGameSettings EasyGameSettings;
			EasyGameSettings.MatrixBoardSize = FIntPoint(9, 9);
			EasyGameSettings.NumberOfMines = 10;
			return EasyGameSettings;
		})());

		_difficultyLevelsSettings.Add(EDifficultyLevel::Intermediate, ([]() -> FMinesweeperGameSettings {
			FMinesweeperGameSettings MediumGameSettings;
			MediumGameSettings.MatrixBoardSize = FIntPoint(16, 16);
			MediumGameSettings.NumberOfMines = 40;
			return MediumGameSettings;
		})());

		_difficultyLevelsSettings.Add(EDifficultyLevel::Expert, ([]() -> FMinesweeperGameSettings {
			FMinesweeperGameSettings MediumGameSettings;
			MediumGameSettings.MatrixBoardSize = FIntPoint(30, 30);
			MediumGameSettings.NumberOfMines = 99;
			return MediumGameSettings;
		})());

		_difficultyLevelsSettings.Add(EDifficultyLevel::Custom, ([]() -> FMinesweeperGameSettings {
			FMinesweeperGameSettings MediumGameSettings;
			MediumGameSettings.MatrixBoardSize = FIntPoint(30, 30);
			MediumGameSettings.NumberOfMines = 150;
			return MediumGameSettings;
		})());
	}

	if (!_gameSettings.IsValid()) {
		FMinesweeperGameSettings DefaultGameSettings = _difficultyLevelsSettings[EDifficultyLevel::Beginner];
		_gameSettings = MakeShared<FMinesweeperGameSettings>(DefaultGameSettings);
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
		// When the game has not been started, we requested to Play the game.
		if (!_gameSession->IsRunning()) {
			// The game view becomes visible as long as the game is being played.
			GameViewBox.Get()->SetVisibility(EVisibility::Visible);

			// The settings become disabled as long as the game is not stopped.
			_numericEntryWidth->SetEnabled(false);
			_numericEntryHeight->SetEnabled(false);
			_numericEntryNumberOfMines->SetEnabled(false);

			// Start the game session with the chosen UI settings.
			StartGameWithCurrentSettings();

			// Update the view for the first time to populate the game view board matrix.
			PopulateGrid();

			return;
		}

		// When the game is running, we requested to Stop the game.
		{
			// Hide the game view since it is not needed until we decide to play the game again.
			GameViewBox.Get()->SetVisibility(EVisibility::Hidden);

			// Enable all the setting entries for being able to setup a new game session.
			_numericEntryWidth->SetEnabled(true);
			_numericEntryHeight->SetEnabled(true);
			_numericEntryNumberOfMines->SetEnabled(true);

			// Clear the matrix cells of the current game session and update the view to clear it.
			_gameSession->GetGameDataState()->ClearMatrixCells();
			PopulateGrid();

			// Shutdown the game session in order to be logically consistent when we will start it up again.
			_gameSession->Shutdown();
		}
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
	check(_gameSettings.IsValid());

	_gameSession->Startup();
	_gameSession->PrepareAndStartGame(*_gameSettings);
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