#include "SMinesweeperGameBoard.h"

#include "Widgets/Layout/SScaleBox.h"

#include "MinesweeperGameUIStyle.h"
#include "SClickableButton.h"
#include "SNumericSettingEntry.h"

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
			SAssignNew(_numericEntryWidth, SNumericSettingEntry<int>)
			.EntryName(TEXT("Width"))
			.ValueGetter([this]() {
				return _gameSettings->MatrixBoardSize.X;
			})
			.ValueSetter([this](int InNewValue) {
				_gameSettings->MatrixBoardSize.X = InNewValue;
			})
			.MinValue(1)
			.MaxValue(80)
		]
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		.AutoHeight()
		.Padding(LateralPadding, 0.0f, LateralPadding, LateralPadding)
		[
			SAssignNew(_numericEntryHeight, SNumericSettingEntry<int>)
			.EntryName(TEXT("Height"))
			.ValueGetter([this]() {
				return _gameSettings->MatrixBoardSize.Y;
			})
			.ValueSetter([this](int InNewValue) {
				_gameSettings->MatrixBoardSize.Y = InNewValue;
			})
			.MinValue(1)
			.MaxValue(80)
		]
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		.AutoHeight()
		.Padding(LateralPadding, 0.0f, LateralPadding, LateralPadding)
		[
			SAssignNew(_numericEntryNumberOfMines, SNumericSettingEntry<int>)
			.EntryName(TEXT("Number Of Mines"))
			.ValueGetter([this, BombsCountValueClamper]() {
				return BombsCountValueClamper(_gameSettings->NumberOfMines);
			})
			.ValueSetter([this, BombsCountValueClamper](int InNewValue) {
				_gameSettings->NumberOfMines = BombsCountValueClamper(InNewValue);
			})
			.MinValue(1)
			.MaxValue(999)
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
				SNew(SBox)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Content()
				[
					SNew(STextBlock)
					.Justification(ETextJustify::Type::Left)
					.MinDesiredWidth(60)
					.ColorAndOpacity(FColor::Red)
					.Font(FMinesweeperGameUIStyle::Get().GetWidgetStyle<FTextBlockStyle>(FName("MinesweeperGameUI.TimerDisplayStyle")).Font)
					.Text_Lambda([this]() { 
						if (!_gameSession->IsRunning()) {
							return FText::FromString(TEXT("Count"));
						}

						const auto GameDataState = _gameSession->GetGameDataState();

						int CountBombs = 0;
						int CountFlagged = 0;

						GameDataState->ForeachCell([&CountFlagged, &CountBombs](const FMinesweeperCellCoordinate&, FMinesweeperCell& InRefCurrentCell) {
							if (InRefCurrentCell.IsFlagged()) {
								CountFlagged++;
							}

							if (InRefCurrentCell.CellState == EMinesweeperCellState::Bomb) {
								CountBombs++;
							}
						});

						return  FText::FromString(FString::FromInt(CountBombs - CountFlagged));
					})
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Center)
			.Padding(0.0f)
			[
				SNew(SBox)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Content()
				[
					SNew(STextBlock)
					.Justification(ETextJustify::Type::Right)
					.MinDesiredWidth(60)
					.ColorAndOpacity(FColor::Red)
					.Font(FMinesweeperGameUIStyle::Get().GetWidgetStyle<FTextBlockStyle>(FName("MinesweeperGameUI.TimerDisplayStyle")).Font)
					.Text_Lambda([this]() { 
						if (!_gameSession->IsRunning()) {
							return FText::FromString(TEXT("Timer"));
						}

						const auto GameDataState = _gameSession->GetGameDataState();
						return  FText::FromString(GameDataState->TickTimer.GetTimeElapsedFromStart().ToString(TEXT("%m:%s")));
					})
				]
			]
		]
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		.AutoHeight()
		.Padding(LateralPadding, 0.0f, LateralPadding, LateralPadding)
		[
			SNew(SButton)
			.Text(FText::FromString(TEXT("Replay")))
			.OnClicked_Lambda([this]() {
				_executeReplay();
				return FReply::Handled();
			})
			.IsEnabled_Lambda([this]() {
				// When a game session is started up but the game is not being played but it has been stopped first.
				return _gameSession->IsRunning() && !_bIsPlaying;
			})
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

void SMinesweeperGameBoard::_executeReplay() {
	// Don't enter _executeReplay() until its existing call finishes first.
	FScopeLock lockReplay(&_mutexReplay);

	check(_gameSession.IsValid());

	const auto Matrix = _gameSession->GetGameDataState()->Matrix;
	const auto ReplayActions = MakeShared<FActionHistory>(_gameSession->GetGameDataState()->ActionHistory);

	if (ReplayActions->Actions.Num() == 0) {
		// Exit when there are no actions to be replayed.
		return;
	}
	
	// Set a convenience time start of the last played game's time start, and restart the timer.
	const auto TimeStart = _timeStart;
	_gameSession->GetGameDataState()->TickTimer.StartTimer();
	
	// Set a convenience replay time start with this new played tick timer's time start.
	const auto ReplayTimeStart = _gameSession->GetGameDataState()->TickTimer.GetTimeStart();

	// Reset all cells to unrevealed, unflagged and unmarked.
	FMinesweeperMatrixNavigator(Matrix.ToSharedRef()).ForeachCell([this](const FMinesweeperCellCoordinate& InCoordinates, FMinesweeperCell& InRefCell) {
		InRefCell.SetRevealed(false);
		InRefCell.SetFlagged(false);
		InRefCell.SetQuestionMarked(false);
	});

	// Reupdated the visual grid with the initial state of the matrix.
	PopulateGrid();

	// Make a guid for this replay execution, empty existing replay executions and add this task guid to the array.
	const auto TaskGuid = FGuid::NewGuid();
	_replayExecutions.Empty();
	_replayExecutions.Add(TaskGuid);

	// Set the boolean for the event of stopping a previous execution of replay.
	_bShouldStopReplay.AtomicSet(false);

	// Run the executions of replay in a parallel thread in order to allow time sleeps.
	AsyncTask(ENamedThreads::AnyThread, [this, ReplayActions, TimeStart, ReplayTimeStart, TaskGuid]() {
		// If this execution has been cleared from the set of execution guids or we should replay as external event
		// then we need to exit this immediately before doing anything.
		{
			FScopeLock lockReplay(&_mutexReplay);
			if (!_replayExecutions.Contains(TaskGuid) || _bShouldStopReplay) {
				return;
			}
		}

		// Calculate the action key time by time subtraction.
		const auto ActionKeyTime = ReplayActions->Actions[0].Time - TimeStart;
		if (ActionKeyTime <= 0) {
			// Skip this, something went wrong and we can't wait for this key time.
			return;
		}

		// Wait until our current timeline tick reaches the target action key time.
		while ((FDateTime::Now() - ReplayTimeStart) < ActionKeyTime);

		// Pop from the first element for each replay action to be executed as a queue.
		while (ReplayActions->Actions.Num() > 0) {
			// If this execution has been cleared from the set of execution guids or we should replay as external event
			// then we should exit this thread execution as soon as possible.
			{
				FScopeLock lockReplay(&_mutexReplay);
				if (!_replayExecutions.Contains(TaskGuid) || _bShouldStopReplay) {
					return;
				}
			}

			// Pop the action from the first element.
			const auto PopAction = ReplayActions->Actions[0];
			ReplayActions->Actions.RemoveAt(0);

			// Perform the next action to replay.
			PopAction.Action->Perform(_gameSession.ToSharedRef(), PopAction.InteractedCell);

			// Update the view with the result of the matrix after the replayed action with respect to the previous state.
			PopulateGrid();

			// If there are more replay actions to come...
			if (ReplayActions->Actions.Num() > 0) {
				// ...take the next action key time to be waited.
				const auto CurrentActionKeyTime = ReplayActions->Actions[0].Time - TimeStart;

				// Wait for the next action key time before going on with its upcoming execution.
				while ((FDateTime::Now() - ReplayTimeStart) < CurrentActionKeyTime);
			}
		}
		
		// Stop the timer when the replay ends.
		_gameSession->GetGameDataState()->TickTimer.StopTimer();
	});

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
		_bShouldStopReplay.AtomicSet(true);
		_bIsPlaying.AtomicSet(false);
		_gameSession->GetGameDataState()->TickTimer.StopTimer();

		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("You Won!"));

		PopulateGrid();
	});

	// When the player loses the game, show a popup and update the view.
	_gameSession->OnGameOver.AddLambda([this]() {
		_bShouldStopReplay.AtomicSet(true);
		_bIsPlaying.AtomicSet(false);
		_gameSession->GetGameDataState()->TickTimer.StopTimer();

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
		_bShouldStopReplay.AtomicSet(true);

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

	_timeStart = _gameSession->GetGameDataState()->TickTimer.GetTimeStart();
	_bIsPlaying.AtomicSet(true);
}

void SMinesweeperGameBoard::PopulateGrid() {
	AsyncTask(ENamedThreads::GameThread, [this]() {
		check(_gameSession.IsValid());

		if (!_gameSession->IsRunning()) {
			return;
		}

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
					SNew(SMinesweeperCell)
					.GameSession(_gameSession)
					.CellCoordinates(InCoordinates)
					.Cell(InRefCell)
					.OnLeftClicked_Lambda([this](const FMinesweeperCellCoordinate& InCellCoordinates, const FMinesweeperCell& InCell) {
						RunAction(FMinesweeperActions::Sweep, InCellCoordinates);
						return FReply::Handled();
					})
					.OnMiddleClicked_Lambda([this](const FMinesweeperCellCoordinate& InCellCoordinates, const FMinesweeperCell& InCell) {
						RunAction(FMinesweeperActions::Mark, InCellCoordinates);
						return FReply::Handled();
					})
					.OnRightClicked_Lambda([this](const FMinesweeperCellCoordinate& InCellCoordinates, const FMinesweeperCell& InCell) {
						RunAction(FMinesweeperActions::Flag, InCellCoordinates);
						return FReply::Handled();
					})
				];
		});
	});
}

void SMinesweeperGameBoard::RunAction(TSharedRef<IMinesweeperAction> InAction, const FMinesweeperCellCoordinate& InCoordinates) {
	check(_gameSession.IsValid());

	_gameSession->RunAction(InAction, InCoordinates);

	PopulateGrid();
}