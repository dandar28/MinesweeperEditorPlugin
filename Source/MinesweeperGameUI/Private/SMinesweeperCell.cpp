#include "SMinesweeperCell.h"

#include "Widgets/Layout/SScaleBox.h"

#include "MinesweeperGameUIStyle.h"
#include "SClickableButton.h"

#include "Minesweeper/FMinesweeperMatrixNavigator.h"

FCellStyle SMinesweeperCell::_getEmptyCellStyle(const FMinesweeperCellCoordinate& InCellCoordinates) const {
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

FLinearColor SMinesweeperCell::_emptyCellColorIntensityDark = FLinearColor(FColor::Cyan) * 0.5;
FLinearColor SMinesweeperCell::_emptyCellColorIntensityLight = FLinearColor(FColor::Cyan) * 0.8;

TArray<FCellStyle> SMinesweeperCell::_emptyCellStylePerBombsAdjacencyCount = TArray<FCellStyle>{
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

FCellStyle SMinesweeperCell::_hiddenCellStyle = FCellStyle()
	.SetBackgroundColor(FLinearColor::White)
	.SetText("");

FCellStyle SMinesweeperCell::_bombCellStyle = FCellStyle()
	.SetBackgroundColor(FLinearColor::Red)
	.SetTextColor(FLinearColor::White)
	.SetText("Bomb");

FCellStyle SMinesweeperCell::_flagCellStyle = FCellStyle()
	.SetBackgroundColor(FLinearColor::Yellow)
	.SetTextColor(FLinearColor::White)
	.SetText("Flag");

FCellStyle SMinesweeperCell::_questionMarkCellStyle = FCellStyle()
	.SetBackgroundColor(FLinearColor(FColor::Cyan))
	.SetTextColor(FLinearColor::White)
	.SetText("?");

void SMinesweeperCell::Construct(const FArguments& InArgs) {
	_gameSession = InArgs._GameSession;
	check(_gameSession.IsValid());

	_cellCoordinates = InArgs._CellCoordinates;
	_cell = InArgs._Cell;

	OnLeftClicked = InArgs._OnLeftClicked;
	OnMiddleClicked = InArgs._OnMiddleClicked;
	OnRightClicked = InArgs._OnRightClicked;

	const auto Matrix = _gameSession->GetGameDataState()->Matrix;

	FCellStyle CurrentCellStyle = _hiddenCellStyle;

	if (InArgs._Cell.IsRevealed()) {
		switch (InArgs._Cell.CellState) {
		case EMinesweeperCellState::Empty:
			CurrentCellStyle = _getEmptyCellStyle(InArgs._CellCoordinates);
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
	} else if (InArgs._Cell.IsFlagged()) {
		CurrentCellStyle = _flagCellStyle;
		CurrentCellStyle.SetContentWidget(
			SNew(SImage)
			.Image(FMinesweeperGameUIStyle::Get().GetBrush("MinesweeperGameUI.Flag"))
		);
	} else if (InArgs._Cell.IsQuestionMarked()) {
		CurrentCellStyle = _questionMarkCellStyle;
		CurrentCellStyle.SetContentWidget(
			SNew(STextBlock)
			.Text(FText::FromString(CurrentCellStyle.Text))
			.ColorAndOpacity(CurrentCellStyle.TextColor)
			.Justification(ETextJustify::Center)
			.Font(FMinesweeperGameUIStyle::Get().GetWidgetStyle<FTextBlockStyle>(FName("MinesweeperGameUI.NumberOfMineStyle")).Font)
		);
	}

	ChildSlot
	[
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
			SNew(SClickableButton)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.ButtonColorAndOpacity(CurrentCellStyle.BackgroundColor)
			.OnLeftClicked_Lambda([this, InArgs]() {
				if (!OnLeftClicked.IsBound()) {
					return FReply::Handled();
				}
				return OnLeftClicked.Execute(InArgs._CellCoordinates, InArgs._Cell);
			})
			.OnMiddleClicked_Lambda([this, InArgs]() {
				if (!OnMiddleClicked.IsBound()) {
					return FReply::Handled();
				}
				return OnMiddleClicked.Execute(InArgs._CellCoordinates, InArgs._Cell);
			})
			.OnRightClicked_Lambda([this, InArgs]() {
				if (!OnRightClicked.IsBound()) {
					return FReply::Handled();
				}
				return OnRightClicked.Execute(InArgs._CellCoordinates, InArgs._Cell);
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
		]
	];
}