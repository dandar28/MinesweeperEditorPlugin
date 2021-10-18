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
 * \brief - Slate widget that renders the settings area with the visual entries
 *			for the game settings.
 */
class MINESWEEPERGAMEUI_API SMinesweeperGameSettings
	: public SCompoundWidget {
public:

	SLATE_BEGIN_ARGS(SMinesweeperGameSettings)
		: _GameSession(nullptr)
	{}
		SLATE_EVENT(FOnClicked, OnPlayButtonClicked)
		SLATE_EVENT(FOnClicked, OnStopButtonClicked)
		SLATE_ARGUMENT(TWeakPtr<FMinesweeperGameSession>, GameSession)
		SLATE_ARGUMENT(TSharedPtr<FMinesweeperGameSettings>, GameSettings)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:

	/**
	 * \brief - Update the difficulty level with a new one.
	 * \param[in] InNewDifficultyLevel - New difficulty level to be set.
	 */
	void _setNewDifficultyLevel(EDifficultyLevel InNewDifficultyLevel);

	/**
	 * \brief - Owned game session that is running on this game board UI widget.
	 */
	TWeakPtr<FMinesweeperGameSession> _gameSession;

	/**
	 * \brief - Shared pointer to the actual instance of the game settings updated from the UI.
	 */
	TSharedPtr<FMinesweeperGameSettings> _gameSettings;
	
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
	 * \brief - Map that binds each difficulty level to its game settings.
	 */
	TMap<EDifficultyLevel, FMinesweeperGameSettings> _difficultyLevelsSettings;

	/**
	 * \brief - Array of options for the possible difficulties to be chosen.
	 */
	static TArray<TSharedPtr<FText>> _staticDifficultyComboOptions;

	/**
	 * \brief - Map that binds each difficulty level to a different default setting instance.
	 */
	static TMap<EDifficultyLevel, FMinesweeperGameSettings> _staticDifficultyLevelsSettings;
};