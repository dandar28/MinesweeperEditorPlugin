#include "StateMachine/FGameStateMachine.h"

#include "StateMachine/ILogicState.h"
#include "StateMachine/FAbstractLogicState.h"

TSharedRef<ILogicState> FGameStateMachine::GetGameLogicState() const {
	return _logicState.ToSharedRef();
}

void FGameStateMachine::GoToState(const TSharedRef<FAbstractLogicState>& InNewState) {
	// If there's a current state, exit from it before entering the next.
	if (_logicState.IsValid()) {
		_logicState->OnExit();
	}

	// Set the new state as the current one.
	_logicState = InNewState;

	//  Set this instance as its owner state machine.
	InNewState->OwnerStateMachine = this->AsShared();

	// Broadcast the event triggered by the change of logic state by passing the new one.
	OnLogicStateChanged.Broadcast(InNewState);

	// Enter the new state.
	InNewState->OnEnter();
}