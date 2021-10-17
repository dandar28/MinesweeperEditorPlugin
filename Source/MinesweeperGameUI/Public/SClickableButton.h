#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

/**
 * \brief - Extended version of SButton which allows to capture events for other mouse buttons
 *			than left button, so that also middle and right button can be listened.
 *			It also handles double click for all the buttons.
 */
class MINESWEEPERGAMEUI_API SClickableButton
	: public SButton {
public:
	SLATE_BEGIN_ARGS(SClickableButton)
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
	{}
		SLATE_EVENT(FOnClicked, OnLeftClicked)
		SLATE_EVENT(FOnClicked, OnMiddleClicked)
		SLATE_EVENT(FOnClicked, OnRightClicked)

		SLATE_EVENT(FOnClicked, OnLeftDoubleClicked)
		SLATE_EVENT(FOnClicked, OnMiddleDoubleClicked)
		SLATE_EVENT(FOnClicked, OnRightDoubleClicked)

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

	FOnClicked OnLeftDoubleClicked;
	FOnClicked OnMiddleDoubleClicked;
	FOnClicked OnRightDoubleClicked;

	void Construct(const FArguments& InArgs);

	FReply OnMouseButtonDown(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;
	FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;
	FReply OnMouseButtonUp(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	virtual FReply _executeOnClick(FOnClicked& InRefDelegate);
	virtual FReply _executeButtonClick(const FKey& InEffectingButton, bool bIsDoubleClick = false);

private:
	FReply _handleMouseButtonDown(const FPointerEvent& InMouseEvent, bool bIsDoubleClick = false);
	FReply _handleMouseButtonUp(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent);
};