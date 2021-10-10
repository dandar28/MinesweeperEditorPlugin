#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

#include "Widgets/Input/SNumericEntryBox.h"

#include "Minesweeper/FMinesweeperGameSession.h"

#include "TNumericSettingEntry.h"

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
	void RunAction(const FMinesweeperCellCoordinate& InCoordinates);

	/**
	 * \brief - When the selected action changes, update the selected action index.
	 */
	void OnSelectedActionChanged(TSharedPtr<FText> InNewItem, ESelectInfo::Type InSelectType);

	/**
	 * \return The selected action option string.
	 */
	FText GetSelectedActionOption() const;

private:
	
	/**
	 * \brief - Make the widget representing the visual appearence for a cell.
	 * \param[in] InCellCoordinates - Coordinates of the target cell.
	 * \param[in] InCell - Target cell to be represented.
	 * \return A slate widget that represents the cell visually.
	 */
	TSharedRef<SWidget> _makeWidgetForCell(
		const FMinesweeperCellCoordinate& InCellCoordinates,
		const FMinesweeperCell& InCell
	);

	/**
	 * \brief - Make a setting entry for numeric type values through a description templated class and a reference to 
	 *			the numeric entry box slate object to get in output.
	 * \param[in] InNumericSettingEntry - Descriptor for the setting entry to be created.
	 * \param[in] OutOwningEntryBox - Output shared pointer to the slate object of the created numeric entry box.
	 * \return The slate object of the horizontal box of the full created setting entry.
	 */
	TSharedRef<SHorizontalBox> _makeNumericSettingEntry(const TNumericSettingEntry<int>& InNumericSettingEntry, TSharedPtr<SNumericEntryBox<int>>& OutOwningEntryBox);

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
	 * \brief - Owned game session that is running on this game board UI widget.
	 */
	TSharedPtr<FMinesweeperGameSession> _gameSession;

	/**
	 * \brief - Grid panel holding the cells in a tabular organization.
	 */
	TSharedPtr<SUniformGridPanel> _cellsGridPanel;

	/**
	 * \brief - Array of options for the possible actions to be performed.
	 */
	const TArray<TSharedPtr<FText>> _actionComboOptions = TArray<TSharedPtr<FText>>{
		MakeShared<FText>(FText::FromString("Sweep")),
		MakeShared<FText>(FText::FromString("Flag"))
	};

	/**
	 * \brief - Function type which takes a game session and a cell coordinate to perform an action on it.
	 */
	using FActionFunction = TFunction<void(TSharedPtr<FMinesweeperGameSession>, const FMinesweeperCellCoordinate&)>;

	/**
	 * \brief - Array of ordered action functions for each option label of _actionComboOptions.
	 */
	const TArray<FActionFunction> _actionFunctions = TArray<FActionFunction>{
		[](auto InGameSession, const auto& InCoordinates) {
			InGameSession->SweepOnCell(InCoordinates);
		},
		[](auto InGameSession, const auto& InCoordinates) {
			InGameSession->FlagOnCell(InCoordinates);
		}
	};

	/**
	 * \brief - Currently selected predefined difficulty level index.
	 */
	int32 _selectedDifficultyIndex = 0;

	/**
	 * \brief - Currently selected action index.
	 */
	int32 _selectedActionIndex = 0;

	/**
	 * \brief - Slate object of the numeric entry of the Width setting.
	 */
	TSharedPtr<SNumericEntryBox<int>> _numericEntryWidth;

	/**
	 * \brief - Slate object of the numeric entry of the Height setting.
	 */
	TSharedPtr<SNumericEntryBox<int>> _numericEntryHeight;

	/**
	 * \brief - Slate object of the numeric entry of the NumberOfMines setting.
	 */
	TSharedPtr<SNumericEntryBox<int>> _numericEntryNumberOfMines;

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