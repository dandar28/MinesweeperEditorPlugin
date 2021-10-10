#pragma once

#include "CoreMinimal.h"

template <typename NumberType>
struct TNumericSettingEntry {
	TNumericSettingEntry<NumberType>& SetEntryName(const FString& InEntryName) {
		EntryName = InEntryName;
		return *this;
	}

	TNumericSettingEntry<NumberType>& SetMinValue(const TOptional<NumberType>& InValue) {
		MinValue = InValue;
		return *this;
	}

	TNumericSettingEntry<NumberType>& SetMaxValue(const TOptional<NumberType>& InValue) {
		MaxValue = InValue;
		return *this;
	}

	TNumericSettingEntry<NumberType>& SetValueGetter(const TFunction<TOptional<NumberType>()>& InFunction) {
		ValueGetter = InFunction;
		return *this;
	}

	TNumericSettingEntry<NumberType>& SetValueSetter(const TFunction<void(NumberType)>& InFunction) {
		ValueSetter = InFunction;
		return *this;
	}

	NumberType Clamp(NumberType InValue) const {
		return FMath::Clamp<NumberType>(InValue, MinValue.Get(InValue), MaxValue.Get(InValue));
	}

	FString EntryName;
	TOptional<NumberType> MinValue;
	TOptional<NumberType> MaxValue;
	TFunction<TOptional<NumberType>()> ValueGetter;
	TFunction<void(NumberType)> ValueSetter;
};