#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

#include "Widgets/Input/SNumericEntryBox.h"

/**
 * \brief - Slate widget representing a setting entry with a label and a numeric value
 *			shown into an horizontal box.
 */
template <typename NumberType>
class MINESWEEPERGAMEUI_API SNumericSettingEntry
	: public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(SNumericSettingEntry)
	{}
		SLATE_ARGUMENT(FString, EntryName)
		SLATE_ARGUMENT(TOptional<NumberType>, MinValue)
		SLATE_ARGUMENT(TOptional<NumberType>, MaxValue)
		SLATE_ARGUMENT(TFunction<TOptional<NumberType>()>, ValueGetter)
		SLATE_ARGUMENT(TFunction<void(NumberType)>, ValueSetter)

		NumberType Clamp(NumberType InValue) const {
			return FMath::Clamp<NumberType>(InValue, _MinValue.Get(InValue), _MaxValue.Get(InValue));
		}
	SLATE_END_ARGS()

	FString EntryName;
	TOptional<NumberType> MinValue;
	TOptional<NumberType> MaxValue;
	TFunction<TOptional<NumberType>()> ValueGetter;
	TFunction<void(NumberType)> ValueSetter;

	void Construct(const FArguments& InArgs) {		
		ChildSlot
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(0.5f)
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.Padding(0.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(InArgs._EntryName))
				.Justification(ETextJustify::Type::Left)
				.MinDesiredWidth(60)
			]
			+ SHorizontalBox::Slot()
			.FillWidth(0.5f)
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.Padding(0.0f)
			[
				SNew(SNumericEntryBox<int>)
				.Value(InArgs._ValueGetter().Get(0))
				.MinValue(InArgs._MinValue)
				.MaxValue(InArgs._MaxValue)
				.MinDesiredValueWidth(200)
				.OnValueCommitted_Lambda([InArgs](int InNewValue, ETextCommit::Type InCommitType) {
					InArgs._ValueSetter(InArgs.Clamp(InNewValue));
				})
				.Value_Lambda([InArgs]() -> TOptional<int32> {
					return InArgs.Clamp(InArgs._ValueGetter().Get(0));
				})
			]
		];
	}
};