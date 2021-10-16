#include "SMinesweeperGameBoard.h"

#include "Widgets/Layout/SScaleBox.h"

#include "MinesweeperGameUIStyle.h"

#include "Minesweeper/FMinesweeperMatrixNavigator.h"

/*
SButtonClickable::SButtonClickable() {

}
*/

FReply SButtonClickable::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = FReply::Unhandled();
	if (IsEnabled())
	{
		Press();
		PressedScreenSpacePosition = MouseEvent.GetScreenSpacePosition();

		EButtonClickMethod::Type InputClickMethod = GetClickMethodFromInputType(MouseEvent);

		if (InputClickMethod == EButtonClickMethod::MouseDown)
		{
			//<<< Get the reply from the execute function
			const auto EffectingButton = MouseEvent.GetEffectingButton();
			if (EffectingButton == EKeys::LeftMouseButton) {
				Reply = ExecuteOnClick(OnLeftClicked);
			} else if (EffectingButton == EKeys::MiddleMouseButton) {
				Reply = ExecuteOnClick(OnMiddleClicked);
			} else if(EffectingButton == EKeys::RightMouseButton) {
				Reply = ExecuteOnClick(OnRightClicked);
			}
			//>>>

			//You should ALWAYS handle the OnClicked event.
			ensure(Reply.IsEventHandled() == true);
		}
		else if (InputClickMethod == EButtonClickMethod::PreciseClick)
		{
			// do not capture the pointer for precise taps or clicks
			// 
			Reply = FReply::Handled();
		}
		else
		{
			//we need to capture the mouse for MouseUp events
			Reply = FReply::Handled().CaptureMouse(AsShared());
		}
	}

	Invalidate(EInvalidateWidget::Layout);

	//return the constructed reply
	return Reply;
}

FReply SButtonClickable::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	return OnMouseButtonDown(InMyGeometry, InMouseEvent);
}

FReply SButtonClickable::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = FReply::Unhandled();
	const EButtonClickMethod::Type InputClickMethod = GetClickMethodFromInputType(MouseEvent);
	const bool bMustBePressed = InputClickMethod == EButtonClickMethod::DownAndUp || InputClickMethod == EButtonClickMethod::PreciseClick;
	const bool bMeetsPressedRequirements = (!bMustBePressed || (bIsPressed && bMustBePressed));

	if (bMeetsPressedRequirements)
	{
		Release();

		if (IsEnabled())
		{
			if (InputClickMethod == EButtonClickMethod::MouseDown)
			{
				// NOTE: If we're configured to click on mouse-down/precise-tap, then we never capture the mouse thus
				//       may never receive an OnMouseButtonUp() call.  We make sure that our bIsPressed
				//       state is reset by overriding OnMouseLeave().
			}
			else
			{
				bool bEventOverButton = IsHovered();

				if (!bEventOverButton && MouseEvent.IsTouchEvent())
				{
					bEventOverButton = MyGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition());
				}

				if (bEventOverButton)
				{
					// If we asked for a precise tap, all we need is for the user to have not moved their pointer very far.
					const bool bTriggerForTouchEvent = InputClickMethod == EButtonClickMethod::PreciseClick;

					// If we were asked to allow the button to be clicked on mouse up, regardless of whether the user
					// pressed the button down first, then we'll allow the click to proceed without an active capture
					const bool bTriggerForMouseEvent = (InputClickMethod == EButtonClickMethod::MouseUp || HasMouseCapture());

					if ((bTriggerForTouchEvent || bTriggerForMouseEvent))
					{
						//<<< Get the reply from the execute function
						const auto EffectingButton = MouseEvent.GetEffectingButton();
						if (EffectingButton == EKeys::LeftMouseButton) {
							Reply = ExecuteOnClick(OnLeftClicked);
						}
						else if (EffectingButton == EKeys::MiddleMouseButton) {
							Reply = ExecuteOnClick(OnMiddleClicked);
						}
						else if (EffectingButton == EKeys::RightMouseButton) {
							Reply = ExecuteOnClick(OnRightClicked);
						}
						//>>>
					}
				}
			}
		}

		//If the user of the button didn't handle this click, then the button's
		//default behavior handles it.
		if (Reply.IsEventHandled() == false)
		{
			Reply = FReply::Handled();
		}
	}

	//If the user hasn't requested a new mouse captor and the button still has mouse capture,
	//then the default behavior of the button is to release mouse capture.
	if (Reply.GetMouseCaptor().IsValid() == false && HasMouseCapture())
	{
		Reply.ReleaseMouseCapture();
	}

	Invalidate(EInvalidateWidget::Layout);

	return Reply;
}

FReply SButtonClickable::ExecuteOnClick(FOnClicked& InRefDelegate)
{
	if (InRefDelegate.IsBound())
	{
		FReply Reply = InRefDelegate.Execute();
#if WITH_ACCESSIBILITY
		FSlateApplicationBase::Get().GetAccessibleMessageHandler()->OnWidgetEventRaised(AsShared(), EAccessibleEvent::Activate);
#endif
		return Reply;
	}
	else
	{
		return FReply::Handled();
	}
}


void SButtonClickable::Construct(const FArguments& InArgs) {
	/**
	* 
		: _Content()
		, _ButtonStyle( &FCoreStyle::Get().GetWidgetStyle< FButtonStyle >( "Button" ) )
		, _TextStyle( &FCoreStyle::Get().GetWidgetStyle< FTextBlockStyle >("NormalText") )
		, _HAlign( HAlign_Fill )
		, _VAlign( VAlign_Fill )
		, _ContentPadding(FMargin(4.0, 2.0))
		, _Text()
		, _ClickMethod( EButtonClickMethod::DownAndUp )
		, _TouchMethod( EButtonTouchMethod::DownAndUp )
		, _PressMethod( EButtonPressMethod::DownAndUp )
		, _DesiredSizeScale( FVector2D(1,1) )
		, _ContentScale( FVector2D(1,1) )
		, _ButtonColorAndOpacity(FLinearColor::White)
		, _ForegroundColor( FCoreStyle::Get().GetSlateColor( "InvertedForeground" ) )
		, _IsFocusable( true )
	 */
	SButton::Construct(SButton::FArguments()
		.ContentScale(InArgs._ContentScale)
		.ContentPadding(InArgs._ContentPadding)
		.DesiredSizeScale(InArgs._DesiredSizeScale)
		.ForegroundColor(InArgs._ForegroundColor)
		.ButtonStyle(InArgs._ButtonStyle)
		.IsFocusable(InArgs._IsFocusable)		
		.OnClicked(InArgs._OnClicked)
		.OnPressed(InArgs._OnPressed)
		.OnReleased(InArgs._OnReleased)
		.OnHovered(InArgs._OnHovered)
		.OnUnhovered(InArgs._OnUnhovered)
		.ClickMethod(InArgs._ClickMethod)
		.TouchMethod(InArgs._TouchMethod)
		.PressMethod(InArgs._PressMethod)
		.HoveredSoundOverride(InArgs._HoveredSoundOverride)//.Get(Style ? Style->HoveredSlateSound : TOptional<FSlateSound>{}))
		.PressedSoundOverride(InArgs._PressedSoundOverride)//.Get(Style ? Style->PressedSlateSound : TOptional<FSlateSound>{}))
		.HAlign(InArgs._HAlign)
		.VAlign(InArgs._VAlign)
		.ButtonColorAndOpacity(InArgs._ButtonColorAndOpacity)
		.Content()
		[
			InArgs._Content.Widget
		]
	);

	OnLeftClicked = InArgs._OnLeftClicked;
	OnMiddleClicked = InArgs._OnMiddleClicked;
	OnRightClicked = InArgs._OnRightClicked;

	OnHovered.BindLambda([this]() {
		// FSlateApplication::GetPressedMouseButtons()
	});
	OnUnhovered.BindLambda([this]() {
	});
}

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
					_makeWidgetForCell(InCoordinates, InRefCell)
				];
		});
	});
}

void SMinesweeperGameBoard::RunAction(TSharedRef<IMinesweeperAction> InAction, const FMinesweeperCellCoordinate& InCoordinates) {
	check(_gameSession.IsValid());

	_gameSession->RunAction(InAction, InCoordinates);

	PopulateGrid();
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
	} else if (InCell.IsQuestionMarked()) {
		CurrentCellStyle = _questionMarkCellStyle;
		CurrentCellStyle.SetContentWidget(
			SNew(STextBlock)
			.Text(FText::FromString(CurrentCellStyle.Text))
			.ColorAndOpacity(CurrentCellStyle.TextColor)
			.Justification(ETextJustify::Center)
			.Font(FMinesweeperGameUIStyle::Get().GetWidgetStyle<FTextBlockStyle>(FName("MinesweeperGameUI.NumberOfMineStyle")).Font)
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
			SNew(SButtonClickable)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.ButtonColorAndOpacity(CurrentCellStyle.BackgroundColor)
			.OnLeftClicked_Lambda([this, InCellCoordinates]() {
				RunAction(FMinesweeperActions::Sweep, InCellCoordinates);
				return FReply::Handled();
			})
			.OnMiddleClicked_Lambda([this, InCellCoordinates]() {
				RunAction(FMinesweeperActions::Mark, InCellCoordinates);
				return FReply::Handled();
			})
			.OnRightClicked_Lambda([this, InCellCoordinates]() {
				RunAction(FMinesweeperActions::Flag, InCellCoordinates);
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