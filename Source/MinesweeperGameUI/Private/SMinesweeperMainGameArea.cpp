#include "SMinesweeperMainGameArea.h"

void SMinesweeperMainGameArea::Construct(const FArguments& InArgs) {
	const auto GameSession = InArgs._GameSession;
	
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
					.Text_Lambda([GameSession]() {
						if (!GameSession.IsValid() || !GameSession.Pin()->IsRunning()) {
							return FText::FromString(TEXT("Count"));
						}

						const auto GameDataState = GameSession.Pin()->GetGameDataState();

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
					.Text_Lambda([GameSession]() {
						if (!GameSession.IsValid() || !GameSession.Pin()->IsRunning()) {
							return FText::FromString(TEXT("Timer"));
						}

						const auto GameDataState = GameSession.Pin()->GetGameDataState();
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
			.IsEnabled_Lambda([GameSession]() {
				// When a game session is started up but the game is not being played but it has been stopped first.
				return GameSession.IsValid() && GameSession.Pin()->IsRunning() && !GameSession.Pin()->IsPlaying();
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