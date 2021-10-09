#pragma once

#include "CoreMinimal.h"

#include "ILogicState.h"

class MINESWEEPERGAMELOGICS_API FAbstractLogicState : public ILogicState {
public:
	virtual ~FAbstractLogicState() = default;

	void OnEnter() override {}
	void OnExit() override {}

	TWeakPtr<class FGameStateMachine> OwnerStateMachine;
};