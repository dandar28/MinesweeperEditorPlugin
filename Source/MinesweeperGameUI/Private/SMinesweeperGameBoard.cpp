#include "SMinesweeperGameBoard.h"

#include "Widgets/Layout/SScaleBox.h"

#include "MinesweeperGameUIStyle.h"
#include "SMinesweeperGameSettings.h"
#include "SMinesweeperMainGameArea.h"

#include "Minesweeper/FMinesweeperMatrixNavigator.h"

void SMinesweeperGameBoard::Construct(const FArguments& InArgs){
	bCanSupportFocus = true;

	// Assign the input game session and check its validity.
	_gameSession = InArgs._GameSession;
	check(_gameSession.IsValid());

	// Create the main shared instance of game settings.
	_gameSettings = MakeShared<FMinesweeperGameSettings>();

	// When the player wins the game, show a popup and update the view.
	_gameSession->OnGameWin.AddLambda([this]() {
		_bShouldStopReplay.AtomicSet(true);
		_gameSession->GetGameDataState()->TickTimer.StopTimer();

		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("You Won!"));

		PopulateGrid();
	});

	// When the player loses the game, show a popup and update the view.
	_gameSession->OnGameOver.AddLambda([this]() {
		_bShouldStopReplay.AtomicSet(true);
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

	TSharedPtr<SMinesweeperMainGameArea> GameViewBox;
	TSharedPtr<SMinesweeperGameSettings> SettingsBox;
	
	SAssignNew(GameViewBox, SMinesweeperMainGameArea)
	.GameSession(_gameSession)
	.OnReplayButtonClicked_Lambda([this]() {
		_executeReplay();
		return FReply::Handled();
	})
	.Content()
	[
		_cellsGridPanel.ToSharedRef()
	];
	
	SAssignNew(SettingsBox, SMinesweeperGameSettings)
	.GameSession(_gameSession)
	.GameSettings(_gameSettings)
	.OnPlayButtonClicked_Lambda([this, GameViewBox]() {
		_bShouldStopReplay.AtomicSet(true);

		// The game view becomes visible as long as the game is being played.
		GameViewBox.Get()->SetVisibility(EVisibility::Visible);

		// Start the game session with the chosen UI settings.
		StartGameWithCurrentSettings();

		// Update the view for the first time to populate the game view board matrix.
		PopulateGrid();

		return FReply::Handled();
	})
	.OnStopButtonClicked_Lambda([this, GameViewBox]() {
		// Hide the game view since it is not needed until we decide to play the game again.
		GameViewBox.Get()->SetVisibility(EVisibility::Hidden);

		// Clear the matrix cells of the current game session and update the view to clear it.
		_gameSession->GetGameDataState()->ClearMatrixCells();
		PopulateGrid();

		// Shutdown the game session in order to be logically consistent when we will start it up again.
		_gameSession->Shutdown();

		return FReply::Handled();
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
	_gameSession->SetSettings(*_gameSettings);
	_gameSession->PlayGame();

	_timeStart = _gameSession->GetGameDataState()->TickTimer.GetTimeStart();
}

void SMinesweeperGameBoard::PopulateGrid() {
	check(_gameSession.IsValid());

	if (!_gameSession->IsRunning()) {
		return;
	}

	const auto Matrix = _gameSession->GetGameDataState()->Matrix;
	const FIntPoint BoardMatrixSize = Matrix->GetSize();

	// Clear the children of the cells grid panel in order to readd all children from zero.
	_cellsGridPanel->ClearChildren();

	// For each cell, add its representative slate widget as child of the cells grid panel.
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
}

void SMinesweeperGameBoard::RunAction(TSharedRef<IMinesweeperAction> InAction, const FMinesweeperCellCoordinate& InCoordinates) {
	check(_gameSession.IsValid());

	_gameSession->RunAction(InAction, InCoordinates);

	PopulateGrid();
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
	AsyncTask(ENamedThreads::AnyThread, [ThisAsShared = this->AsShared(), this, ReplayActions, TimeStart, ReplayTimeStart, TaskGuid]() {
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

			AsyncTask(ENamedThreads::GameThread, [ThisAsShared, this, PopAction]() {
				// Perform the next action to replay.
				PopAction.Action->Perform(_gameSession.ToSharedRef(), PopAction.InteractedCell);

				// Update the view with the result of the matrix after the replayed action with respect to the previous state.
				PopulateGrid();
			});

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