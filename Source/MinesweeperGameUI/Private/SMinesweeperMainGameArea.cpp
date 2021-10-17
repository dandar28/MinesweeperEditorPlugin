#include "SMinesweeperMainGameArea.h"

void SMinesweeperMainGameArea::Construct(const FArguments& InArgs) {
	_gameSession = InArgs._GameSession;
	
	const float LateralPadding = 12.0f;
	ChildSlot
	[
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
			.OnClicked(InArgs._OnReplayButtonClicked)
			.IsEnabled_Lambda([this]() {
				// When a game session is started up but the game is not being played but it has been stopped first.
				return _gameSession->IsRunning() && !_gameSession->IsPlaying();
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
					InArgs._Content.Widget
				]
			]
		]
	];
}