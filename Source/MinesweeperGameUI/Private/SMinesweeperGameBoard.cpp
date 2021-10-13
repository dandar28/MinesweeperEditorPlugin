#include "SMinesweeperGameBoard.h"

#include "Widgets/Layout/SScaleBox.h"

#include "MinesweeperGameUIStyle.h"

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
			SAssignNew(_difficultyComboBox, SComboBox<TSharedPtr<FText>>)
			.OptionsSource(&_difficultyComboOptions)
			.IsEnabled(true)
			.OnGenerateWidget_Lambda([](TSharedPtr<FText> Item)
			{
				return SNew(STextBlock).Text(*Item);
			})
			.Content()
			[
				SNew(STextBlock)
				.Text_Lambda([this]() -> FText {
					return *_difficultyComboOptions[_selectedDifficultyIndex].Get();
				})
			]
			.OnSelectionChanged_Lambda([this](TSharedPtr<FText> InNewItem, ESelectInfo::Type InSelectType) {
				const EDifficultyLevel NewDifficultyLevel = (EDifficultyLevel)_difficultyComboOptions.IndexOfByPredicate([InNewItem](TSharedPtr<FText> InElement) {
					return InElement->ToString() == InNewItem->ToString();
				});

				_setNewDifficultyLevel(NewDifficultyLevel);
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
			.FillWidth(1.f)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Center)
			.Padding(0.0f)
			[
				SNew(STextBlock)
				.Text_Lambda([this]() { 
					if (!_gameSession->IsRunning()) {
						return FText::FromString(TEXT("Timer"));
					}

					const auto GameDataState = _gameSession->GetGameDataState();
					return  FText::FromString(GameDataState->TickTimer.GetTimeElapsedFromStart().ToString());
				})
				.Justification(ETextJustify::Type::Center)
				.MinDesiredWidth(60)
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Center)
			.Padding(0.0f)
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

void SMinesweeperGameBoard::_setNewDifficultyLevel(EDifficultyLevel InNewDifficultyLevel) {
	// Update custom settings with the ones that we just left.
	const EDifficultyLevel CurrentDifficultyLevel = (EDifficultyLevel)_selectedDifficultyIndex;
	if (CurrentDifficultyLevel == EDifficultyLevel::Custom) {
		_difficultyLevelsSettings[EDifficultyLevel::Custom] = (*_gameSettings);
	}

	// Update the index with the new selected one.
	_selectedDifficultyIndex = (int)InNewDifficultyLevel;

	// Update the game settings with the ones of the selected difficulty level.
	const EDifficultyLevel NewDifficultyLevel = (EDifficultyLevel)_selectedDifficultyIndex;
	const FMinesweeperGameSettings& PredefinedGameSettings = _difficultyLevelsSettings[NewDifficultyLevel];
	(*_gameSettings) = PredefinedGameSettings;

	// Enable settings widgets only when we are in Custom difficulty mode.
	const bool bIsNewDifficultyCustom = NewDifficultyLevel == EDifficultyLevel::Custom;
	_numericEntryWidth->SetEnabled(bIsNewDifficultyCustom);
	_numericEntryHeight->SetEnabled(bIsNewDifficultyCustom);
	_numericEntryNumberOfMines->SetEnabled(bIsNewDifficultyCustom);
}

void SMinesweeperGameBoard::Construct(const FArguments& InArgs){
	bCanSupportFocus = true;

	// Assign the input game session and check its validity.
	_gameSession = InArgs._GameSession;
	check(_gameSession.IsValid());

	// Populate all difficulty levels' settings.
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
		_gameSettings = MakeShared<FMinesweeperGameSettings>();
	}

	// When the player wins the game, show a popup and update the view.
	_gameSession->OnGameWin.AddLambda([this]() {
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("You Won!"));

		PopulateGrid();
	});

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
			_difficultyComboBox->SetEnabled(false);

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

			// Retrieve current difficulty level because enabling the other settings depends on whether we are in Custom difficulty mode or not.
			const EDifficultyLevel CurrentDifficultyLevel = (EDifficultyLevel)_selectedDifficultyIndex;
			const bool bIsCurrentDifficultyCustom = CurrentDifficultyLevel == EDifficultyLevel::Custom;

			// Enable all the setting entries for being able to setup a new game session.
			_numericEntryWidth->SetEnabled(bIsCurrentDifficultyCustom);
			_numericEntryHeight->SetEnabled(bIsCurrentDifficultyCustom);
			_numericEntryNumberOfMines->SetEnabled(bIsCurrentDifficultyCustom);
			_difficultyComboBox->SetEnabled(true);

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

	_setNewDifficultyLevel(EDifficultyLevel::Beginner);

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
	_gameSession->SetSettings(*_gameSettings);
	_gameSession->PlayGame();
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

FCellStyle SMinesweeperGameBoard::_getEmptyCellStyle(const FMinesweeperCellCoordinate& InCellCoordinates) const {
	check(_gameSession.IsValid());

	const auto Matrix = _gameSession->GetGameDataState()->Matrix;

	ensure(_emptyCellStylePerBombsAdjacencyCount.Num() == 9);

	const int CountOfAdjacentBombs = FMinesweeperMatrixNavigator(Matrix.ToSharedRef()).CountAdjacentBombs(InCellCoordinates);
	FCellStyle OutStyle = _emptyCellStylePerBombsAdjacencyCount[CountOfAdjacentBombs];
	if (CountOfAdjacentBombs > 0) {
		OutStyle.SetText(FString::FromInt(CountOfAdjacentBombs));
	}

	return OutStyle;
}

FLinearColor SMinesweeperGameBoard::_emptyCellColorIntensityDark = FLinearColor(FColor::Cyan) * 0.5;
FLinearColor SMinesweeperGameBoard::_emptyCellColorIntensityLight = FLinearColor(FColor::Cyan) * 0.8;

TArray<FCellStyle> SMinesweeperGameBoard::_emptyCellStylePerBombsAdjacencyCount = TArray<FCellStyle>{
	FCellStyle()
		.SetBackgroundColor(_emptyCellColorIntensityDark)
		.SetTextColor(FLinearColor::White),

	FCellStyle()
		.SetBackgroundColor(_emptyCellColorIntensityLight)
		.SetTextColor(FLinearColor::Blue),

	FCellStyle()
		.SetBackgroundColor(_emptyCellColorIntensityLight)
		.SetTextColor(FLinearColor::Green),

	FCellStyle()
		.SetBackgroundColor(_emptyCellColorIntensityLight)
		.SetTextColor(FLinearColor::Red),

	FCellStyle()
		.SetBackgroundColor(_emptyCellColorIntensityLight)
		.SetTextColor(FLinearColor::Red),

	FCellStyle()
		.SetBackgroundColor(_emptyCellColorIntensityLight)
		.SetTextColor(FLinearColor::Red),

	FCellStyle()
		.SetBackgroundColor(_emptyCellColorIntensityLight)
		.SetTextColor(FLinearColor::Red),

	FCellStyle()
		.SetBackgroundColor(_emptyCellColorIntensityLight)
		.SetTextColor(FLinearColor::Red),

	FCellStyle()
		.SetBackgroundColor(_emptyCellColorIntensityLight)
		.SetTextColor(FLinearColor::Red)
};

FCellStyle SMinesweeperGameBoard::_hiddenCellStyle = FCellStyle()
	.SetBackgroundColor(FLinearColor::White)
	.SetText("");

FCellStyle SMinesweeperGameBoard::_bombCellStyle = FCellStyle()
	.SetBackgroundColor(FLinearColor::Red)
	.SetTextColor(FLinearColor::White)
	.SetText("Bomb");

FCellStyle SMinesweeperGameBoard::_flagCellStyle = FCellStyle()
	.SetBackgroundColor(FLinearColor::Yellow)
	.SetTextColor(FLinearColor::White)
	.SetText("Flag");

FCellStyle SMinesweeperGameBoard::_questionMarkCellStyle = FCellStyle()
	.SetBackgroundColor(FLinearColor(FColor::Cyan))
	.SetTextColor(FLinearColor::White)
	.SetText("?");

TSharedRef<SWidget> SMinesweeperGameBoard::_makeWidgetForCell(const FMinesweeperCellCoordinate& InCellCoordinates, const FMinesweeperCell& InCell) {
	check(_gameSession.IsValid());

	const auto Matrix = _gameSession->GetGameDataState()->Matrix;

	FCellStyle CurrentCellStyle = _hiddenCellStyle;

	if (InCell.IsRevealed()) {
		switch (InCell.CellState) {
		case EMinesweeperCellState::Empty:
			CurrentCellStyle = _getEmptyCellStyle(InCellCoordinates);
			CurrentCellStyle.SetContentWidget(
				SNew(STextBlock)
					.Text(FText::FromString(CurrentCellStyle.Text))
					.ColorAndOpacity(CurrentCellStyle.TextColor)
					.Justification(ETextJustify::Center)
					.Font(FMinesweeperGameUIStyle::Get().GetWidgetStyle<FTextBlockStyle>(FName("MinesweeperGameUI.NumberOfMineStyle")).Font)
			);

			break;
		case EMinesweeperCellState::Bomb:
			CurrentCellStyle = _bombCellStyle;
			CurrentCellStyle.SetContentWidget(
				SNew(SImage)
				.Image(FMinesweeperGameUIStyle::Get().GetBrush("MinesweeperGameUI.Bomb"))
			);

			break;
		}
	} else if (InCell.IsFlagged()) {
		CurrentCellStyle = _flagCellStyle;
		CurrentCellStyle.SetContentWidget(
			SNew(SImage)
			.Image(FMinesweeperGameUIStyle::Get().GetBrush("MinesweeperGameUI.Flag"))
		);
	}

	return
		SNew(SBox)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.WidthOverride(40)
		.HeightOverride(40)
		.MinDesiredWidth(40)
		.MinDesiredHeight(40)
		.MaxDesiredWidth(40)
		.MaxDesiredHeight(40)
		.MaxAspectRatio(1)
		.MinAspectRatio(1)
		.Content()
		[
			SNew(SButton)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.ButtonColorAndOpacity(CurrentCellStyle.BackgroundColor)
			.OnClicked_Lambda([this, InCellCoordinates]() {
				RunAction(InCellCoordinates);
				return FReply::Handled();
			})
			.Content()
			[
				SNew(SScaleBox)
				.StretchDirection(EStretchDirection::Both)
				.Stretch(EStretch::ScaleToFit)
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				.VAlign(EVerticalAlignment::VAlign_Center)
				[
					CurrentCellStyle.ContentWidget.IsValid() ? CurrentCellStyle.ContentWidget.ToSharedRef() : SNew(SBox)
				]
			]
		];
}