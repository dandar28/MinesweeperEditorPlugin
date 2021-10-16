#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

#include "SNumericSettingEntry.h"
#include "SMinesweeperCell.h"

#include "Minesweeper/FMinesweeperGameSession.h"

/**
 * \brief - Enum containing the available difficulty levels settable for the game.
 */
enum EDifficultyLevel {
	Beginner,
	Intermediate,
	Expert,
	Custom
};

/**
 * \brief - Slate widget that renders a minesweeper game board visually and handles the
 *			game logic bindings with the UI updates through an instanced game session.
 */
class MINESWEEPERGAMEUI_API SMinesweeperGameBoard
	: public SCompoundWidget {
public:

	SLATE_BEGIN_ARGS(SMinesweeperGameBoard)
		: _GameSession(nullptr)
	{}
		SLATE_ARGUMENT( TSharedPtr<FMinesweeperGameSession>, GameSession )
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	bool SupportsKeyboardFocus() const override;

	/**
	 * \brief - Startup the game with the current game settings chosen through the UI.
	 */
	void StartGameWithCurrentSettings();

	/**
	 * \brief - Populate the grid matrix visually by regenerating it from the current state of the game session.
	 */
	void PopulateGrid();

	/**
	 * \brief - Run the selected action from the UI to the target coordinates.
	 * \param[in] InCoordinates - Target coordinates to run the action on.
	 */
	void RunAction(TSharedRef<IMinesweeperAction> InAction, const FMinesweeperCellCoordinate& InCoordinates);

private:
	/**
	 * \brief - Make the whole area of the settings.
	 * \param[in] InPlayButtonClicked - Callback for the OnClicked event of the PlayStop button.
	 * \return The slate object of the vertical box of the full created settings area.
	 */
	TSharedRef<SVerticalBox> _makeSettingsArea(const TFunction<void()>& InPlayButtonClicked);

	/**
	 * \brief - Make the whole area of the game view where the game is played.
	 * \return The slate object of the vertical box of the full created game area.
	 */
	TSharedRef<SVerticalBox> _makeMainGameArea();

	/**
	 * \brief - Update the difficulty level with a new one.
	 * \param[in] InNewDifficultyLevel - New difficulty level to be set.
	 */
	void _setNewDifficultyLevel(EDifficultyLevel InNewDifficultyLevel);

	/**
	 * \brief - This methods shows and plays the Replay of the actions history so far.
	 */
	void _executeReplay();

	/**
	 * \brief - Owned game session that is running on this game board UI widget.
	 */
	TSharedPtr<FMinesweeperGameSession> _gameSession;

	/**
	 * \brief - Grid panel holding the cells in a tabular organization.
	 */
	TSharedPtr<SUniformGridPanel> _cellsGridPanel;

	/**
	 * \brief - True when a game is being played and it is not finished yet.
	 */
	FThreadSafeBool _bIsPlaying = false;

	/**
	 * \brief - Last play's time start.
	 */
	FDateTime _timeStart;

	/**
	 * \brief - Existing replaying display executions.
	 */
	TSet<FGuid> _replayExecutions;

	/**
	 * \brief - Critical section for mutex access to replay method call.
	 */
	FCriticalSection _mutexReplay;

	/**
	 * \brief - When this is set to true, existing replaying displays are stopped.
	 */
	FThreadSafeBool _bShouldStopReplay = false;
	
	/**
	 * \brief - Currently selected predefined difficulty level index.
	 */
	int32 _selectedDifficultyIndex = 0;

	/**
	 * \brief - Slate object of the numeric entry of the Width setting.
	 */
	TSharedPtr<SNumericSettingEntry<int>> _numericEntryWidth;

	/**
	 * \brief - Slate object of the numeric entry of the Height setting.
	 */
	TSharedPtr<SNumericSettingEntry<int>> _numericEntryHeight;

	/**
	 * \brief - Slate object of the numeric entry of the NumberOfMines setting.
	 */
	TSharedPtr<SNumericSettingEntry<int>> _numericEntryNumberOfMines;

	/**
	 * \brief - Slate object of the difficulty combo box setting.
	 */
	TSharedPtr<SComboBox<TSharedPtr<FText>>> _difficultyComboBox;

	/**
	 * \brief - Shared pointer to the actual instance of the game settings updated from the UI.
	 */
	TSharedPtr<FMinesweeperGameSettings> _gameSettings;

	/**
	 * \brief - Array of options for the possible difficulties to be chosen.
	 */
	const TArray<TSharedPtr<FText>> _difficultyComboOptions = TArray<TSharedPtr<FText>>{
		MakeShared<FText>(FText::FromString("Beginner")),
		MakeShared<FText>(FText::FromString("Intermediate")),
		MakeShared<FText>(FText::FromString("Expert")),
		MakeShared<FText>(FText::FromString("Custom"))
	};

	/**
	 * \brief - Map that binds each difficulty level to a different default setting instance.
	 */
	TMap<EDifficultyLevel, FMinesweeperGameSettings> _difficultyLevelsSettings;
};