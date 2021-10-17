#include "SMinesweeperGameSettings.h"

#include "SNumericSettingEntry.h"

void SMinesweeperGameSettings::Construct(const FArguments& InArgs) {
	_gameSession = InArgs._GameSession;
	_gameSettings = InArgs._GameSettings;
	if (!_gameSettings.IsValid()) {
		_gameSettings = MakeShared<FMinesweeperGameSettings>();
	}

	const auto BombsCountValueClamper = [this](int InValue) -> int {
		const auto MatrixSize = _gameSettings->MatrixBoardSize;
		const int MatrixBoardArea = MatrixSize.X * MatrixSize.Y;
		return FMath::Clamp<int>(InValue, 1, MatrixBoardArea);
	};

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

	const float LateralPadding = 12.0f;
	ChildSlot
	[
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
				.OnClicked_Lambda([this, InArgs]() -> FReply {
					// When the game has not been started, we requested to Play the game.
					const bool bHasRequestedPlay = !_gameSession->IsRunning();
					const FOnClicked& OnClickedEvent = bHasRequestedPlay ? InArgs._OnPlayButtonClicked : InArgs._OnStopButtonClicked;
					const FReply Reply = OnClickedEvent.IsBound() ? OnClickedEvent.Execute() : FReply::Unhandled();

					if (!Reply.IsEventHandled()) {
						return Reply;
					}

					// If we requested to Play the game...
					if (bHasRequestedPlay) {
						// ...the settings become disabled as long as the game is not stopped.
						_numericEntryWidth->SetEnabled(false);
						_numericEntryHeight->SetEnabled(false);
						_numericEntryNumberOfMines->SetEnabled(false);
						_difficultyComboBox->SetEnabled(false);

						return Reply;
					}

					// If we requested to Stop the game...
					{
						// Retrieve current difficulty level because enabling the other settings depends on whether we are in Custom difficulty mode or not.
						const EDifficultyLevel CurrentDifficultyLevel = (EDifficultyLevel)_selectedDifficultyIndex;
						const bool bIsCurrentDifficultyCustom = CurrentDifficultyLevel == EDifficultyLevel::Custom;

						// Enable all the setting entries for being able to setup a new game session.
						_numericEntryWidth->SetEnabled(bIsCurrentDifficultyCustom);
						_numericEntryHeight->SetEnabled(bIsCurrentDifficultyCustom);
						_numericEntryNumberOfMines->SetEnabled(bIsCurrentDifficultyCustom);
						_difficultyComboBox->SetEnabled(true);
					}

					return Reply;
				})
			]
		]
	];

	_setNewDifficultyLevel(EDifficultyLevel::Beginner);
}

void SMinesweeperGameSettings::_setNewDifficultyLevel(EDifficultyLevel InNewDifficultyLevel) {
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