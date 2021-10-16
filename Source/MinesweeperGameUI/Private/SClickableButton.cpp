#include "SClickableButton.h"

FReply SClickableButton::_executeOnClick(FOnClicked& InRefDelegate) {
	if (InRefDelegate.IsBound()) {
		FReply Reply = InRefDelegate.Execute();
#if WITH_ACCESSIBILITY
		FSlateApplicationBase::Get().GetAccessibleMessageHandler()->OnWidgetEventRaised(AsShared(), EAccessibleEvent::Activate);
#endif
		return Reply;
	} else {
		return FReply::Handled();
	}
}

FReply SClickableButton::_executeButtonClick(const FKey& InEffectingButton, bool bIsDoubleClick) {
	FReply OutReply = FReply::Unhandled();

	//<<< Get the reply from the execute function
	if (InEffectingButton == EKeys::LeftMouseButton) {
		OutReply = _executeOnClick(bIsDoubleClick ? OnLeftDoubleClicked : OnLeftClicked);
	} else if (InEffectingButton == EKeys::MiddleMouseButton) {
		OutReply = _executeOnClick(bIsDoubleClick ? OnMiddleDoubleClicked : OnMiddleClicked);
	} else if (InEffectingButton == EKeys::RightMouseButton) {
		OutReply = _executeOnClick(bIsDoubleClick ? OnRightDoubleClicked : OnRightClicked);
	}
	//>>>

	return OutReply;
}

FReply SClickableButton::_handleMouseButtonDown(const FPointerEvent& InMouseEvent, bool bIsDoubleClick) {
	// If the button is not enabled, invalidate and reply unhandled.
	if (!IsEnabled()) {
		Invalidate(EInvalidateWidget::Layout);
		return FReply::Unhandled();
	}

	// Press the button.
	Press();

	// Store the location where the button was pressed.
	PressedScreenSpacePosition = InMouseEvent.GetScreenSpacePosition();

	//<<< Detect how to handle the event and obtain the reply.
	FReply Reply = FReply::Unhandled();
	{
		const EButtonClickMethod::Type InputClickMethod = GetClickMethodFromInputType(InMouseEvent);

		switch (InputClickMethod) {
		case EButtonClickMethod::MouseDown:
			// Let the eventually bound delegate handle the event.
			Reply = _executeButtonClick(InMouseEvent.GetEffectingButton(), bIsDoubleClick);
			break;
		case EButtonClickMethod::PreciseClick:
			// Handle without capturing the mouse.
			Reply = FReply::Handled();
			break;
		case EButtonClickMethod::MouseUp:
		case EButtonClickMethod::DownAndUp:
			if (bIsDoubleClick) {
				// Let the eventually bound delegate handle the event.
				Reply = _executeButtonClick(InMouseEvent.GetEffectingButton(), true);
			} else {
				// Handle and capture the mouse for mouse up events.
				Reply = FReply::Handled().CaptureMouse(AsShared());
			}
			break;
		}
	}
	//>>>

	// Ensure that the click event has been handled.
	ensure(Reply.IsEventHandled());

	return Reply;
}

FReply SClickableButton::_handleMouseButtonUp(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) {
	FReply Reply = FReply::Unhandled();
	const EButtonClickMethod::Type InputClickMethod = GetClickMethodFromInputType(InMouseEvent);
	
	// Create a lambda for determining whether or not we should trigger this event.
	auto ShouldTriggerEvent = [this, InputClickMethod, InMyGeometry, InMouseEvent]() -> bool {
		// If not enabled, we skip any behaviour.
		if (!IsEnabled()) {
			return false;
		}

		// If the input click method is mouse down...
		if (InputClickMethod == EButtonClickMethod::MouseDown) {
			// ... we can skip it because we are handling mouse up event.
			return false;
		}

		// If the input click method is precise click...
		if (InputClickMethod == EButtonClickMethod::PreciseClick) {
			// ... we can trigger the event if this is a touch event under location.
			const bool bIsTouchEventOverCursor = InMouseEvent.IsTouchEvent() && InMyGeometry.IsUnderLocation(InMouseEvent.GetScreenSpacePosition());
			if (bIsTouchEventOverCursor) {
				return true;
			}
		}

		// If the mouse button is hovered...
		if (IsHovered()) {
			// ... we want to trigger it if we are handling a mouse up event or if mouse was captured by this button.
			const bool bTriggerForMouseEvent = InputClickMethod == EButtonClickMethod::MouseUp || HasMouseCapture();
			if (bTriggerForMouseEvent) {
				return true;
			}
		}

		// If we did not detect any way to trigger the event so far, let's not trigger it.
		return false;
	};

	const bool bMustBePressed = InputClickMethod == EButtonClickMethod::DownAndUp || InputClickMethod == EButtonClickMethod::PreciseClick;
	const bool bMeetsPressedRequirements = (!bMustBePressed || (bIsPressed && bMustBePressed));
	if (bMeetsPressedRequirements)
	{
		Release();

		// If the event should be triggered for some reason, then execute the bound behaviour
		// to handle it and store its reply.
		if (ShouldTriggerEvent()) {
			Reply = _executeButtonClick(InMouseEvent.GetEffectingButton(), false);
		}

		// If the user of the button didn't handle this click, then the button's
		// default behavior handles it.
		if (!Reply.IsEventHandled())
		{
			Reply = FReply::Handled();
		}
	}

	// If the user hasn't requested a new mouse captor and the button still has mouse capture,
	// then the default behavior of the button is to release mouse capture.
	if (!Reply.GetMouseCaptor().IsValid() && HasMouseCapture())
	{
		Reply.ReleaseMouseCapture();
	}

	Invalidate(EInvalidateWidget::Layout);

	return Reply;
}

FReply SClickableButton::OnMouseButtonDown(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) {
	return _handleMouseButtonDown(InMouseEvent, false);
}

FReply SClickableButton::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) {
	return _handleMouseButtonDown(InMouseEvent, true);
}

FReply SClickableButton::OnMouseButtonUp(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) {
	return _handleMouseButtonUp(InMyGeometry, InMouseEvent);
}

void SClickableButton::Construct(const FArguments& InArgs) {
	const auto OptionalHoveredSound = Style ? InArgs._HoveredSoundOverride.Get(Style->HoveredSlateSound) : InArgs._HoveredSoundOverride;
	const auto OptionalPressedSound = Style ? InArgs._PressedSoundOverride.Get(Style->PressedSlateSound) : InArgs._PressedSoundOverride;
	
	SButton::Construct(SButton::FArguments()
		.ButtonStyle(InArgs._ButtonStyle)
		.TextStyle(InArgs._TextStyle)
		.HAlign(InArgs._HAlign)
		.VAlign(InArgs._VAlign)
		.ContentPadding(InArgs._ContentPadding)
		.Text(InArgs._Text)
		.ClickMethod(InArgs._ClickMethod)
		.TouchMethod(InArgs._TouchMethod)
		.PressMethod(InArgs._PressMethod)
		.DesiredSizeScale(InArgs._DesiredSizeScale)
		.ContentScale(InArgs._ContentScale)
		.ButtonColorAndOpacity(InArgs._ButtonColorAndOpacity)
		.ForegroundColor(InArgs._ForegroundColor)
		.IsFocusable(InArgs._IsFocusable)
		.OnClicked(InArgs._OnClicked)
		.OnPressed(InArgs._OnPressed)
		.OnReleased(InArgs._OnReleased)
		.OnHovered(InArgs._OnHovered)
		.OnUnhovered(InArgs._OnUnhovered)
		.HoveredSoundOverride(OptionalHoveredSound)
		.PressedSoundOverride(OptionalPressedSound)
		.Content()
		[
			InArgs._Content.Widget
		]
	);

	OnLeftClicked = InArgs._OnLeftClicked;
	OnMiddleClicked = InArgs._OnMiddleClicked;
	OnRightClicked = InArgs._OnRightClicked;
}