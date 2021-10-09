#pragma once

#include "CoreMinimal.h"

#include "ILogicState.h"

/**
 * \brief - Abstract logic state that also has a weak reference to its owner state machine.
 */
class MINESWEEPERGAMELOGICS_API FAbstractLogicState
	: public ILogicState {
public:
	virtual ~FAbstractLogicState() = default;

	void OnEnter() override {}
	void OnExit() override {}

	TWeakPtr<class FGameStateMachine> OwnerStateMachine;
};