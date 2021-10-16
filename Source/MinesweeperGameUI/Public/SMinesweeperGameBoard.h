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
 * \brief - Structure which represent the style data for a cell.
 */
struct FCellStyle {
	FCellStyle& SetTextColor(const FLinearColor& InColor) {
		TextColor = InColor;
		return *this;
	}
	FCellStyle& SetBackgroundColor(const FLinearColor& InColor) {
		BackgroundColor = InColor;
		return *this;
	}
	FCellStyle& SetText(const FString& InText) {
		Text = InText;
		return *this;
	}
	FCellStyle& SetContentWidget(const TSharedPtr<SWidget> InWidget) {
		ContentWidget = InWidget;
		return *this;
	}

	TSharedPtr<SWidget> ContentWidget;
	FLinearColor TextColor = FLinearColor::Black;
	FLinearColor BackgroundColor = FLinearColor::White;
	FString Text;
};

class MINESWEEPERGAMEUI_API SButtonClickable
	: public SButton {
public:
	SLATE_BEGIN_ARGS(SButtonClickable)
		: _Content()
		, _ButtonStyle(&FCoreStyle::Get().GetWidgetStyle< FButtonStyle >("Button"))
		, _TextStyle(&FCoreStyle::Get().GetWidgetStyle< FTextBlockStyle >("NormalText"))
		, _HAlign(HAlign_Fill)
		, _VAlign(VAlign_Fill)
		, _ContentPadding(FMargin(4.0, 2.0))
		, _Text()
		, _ClickMethod(EButtonClickMethod::DownAndUp)
		, _TouchMethod(EButtonTouchMethod::DownAndUp)
		, _PressMethod(EButtonPressMethod::DownAndUp)
		, _DesiredSizeScale(FVector2D(1, 1))
		, _ContentScale(FVector2D(1, 1))
		, _ButtonColorAndOpacity(FLinearColor::White)
		, _ForegroundColor(FCoreStyle::Get().GetSlateColor("InvertedForeground"))
		, _IsFocusable(true)
	{
	}

		SLATE_EVENT(FOnClicked, OnLeftClicked)
		SLATE_EVENT(FOnClicked, OnMiddleClicked)
		SLATE_EVENT(FOnClicked, OnRightClicked)

		SLATE_DEFAULT_SLOT(FArguments, Content)
		SLATE_STYLE_ARGUMENT(FButtonStyle, ButtonStyle)
		SLATE_STYLE_ARGUMENT(FTextBlockStyle, TextStyle)
		SLATE_ARGUMENT(EHorizontalAlignment, HAlign)
		SLATE_ARGUMENT(EVerticalAlignment, VAlign)
		SLATE_ATTRIBUTE(FMargin, ContentPadding)
		SLATE_ATTRIBUTE(FText, Text)
		SLATE_EVENT(FOnClicked, OnClicked)
		SLATE_EVENT(FSimpleDelegate, OnPressed)
		SLATE_EVENT(FSimpleDelegate, OnReleased)
		SLATE_EVENT(FSimpleDelegate, OnHovered)
		SLATE_EVENT(FSimpleDelegate, OnUnhovered)
		SLATE_ARGUMENT(EButtonClickMethod::Type, ClickMethod)
		SLATE_ARGUMENT(EButtonTouchMethod::Type, TouchMethod)
		SLATE_ARGUMENT(EButtonPressMethod::Type, PressMethod)
		SLATE_ATTRIBUTE(FVector2D, DesiredSizeScale)
		SLATE_ATTRIBUTE(FVector2D, ContentScale)
		SLATE_ATTRIBUTE(FSlateColor, ButtonColorAndOpacity)
		SLATE_ATTRIBUTE(FSlateColor, ForegroundColor)
		SLATE_ARGUMENT(bool, IsFocusable)
		SLATE_ARGUMENT(TOptional<FSlateSound>, PressedSoundOverride)
		SLATE_ARGUMENT(TOptional<FSlateSound>, HoveredSoundOverride)
		SLATE_ARGUMENT(TOptional<ETextShapingMethod>, TextShapingMethod)
		SLATE_ARGUMENT(TOptional<ETextFlowDirection>, TextFlowDirection)
	SLATE_END_ARGS()

	FOnClicked OnLeftClicked;
	FOnClicked OnMiddleClicked;
	FOnClicked OnRightClicked;

	// SButtonClickable();
	void Construct(const FArguments& InArgs);

	FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;
	FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	FReply ExecuteOnClick(FOnClicked& InRefDelegate);
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

	FCellStyle _getEmptyCellStyle(const FMinesweeperCellCoordinate& InCellCoordinates) const;

	static TArray<FCellStyle> _emptyCellStylePerBombsAdjacencyCount;
	static FCellStyle _hiddenCellStyle;
	static FCellStyle _bombCellStyle;
	static FCellStyle _flagCellStyle;
	static FCellStyle _questionMarkCellStyle;

	static FLinearColor _emptyCellColorIntensityDark;
	static FLinearColor _emptyCellColorIntensityLight;
	
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