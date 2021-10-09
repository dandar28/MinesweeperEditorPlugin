#include "StateMachine/FGameStateMachine.h"

#include "StateMachine/ILogicState.h"
#include "StateMachine/FAbstractLogicState.h"

TSharedRef<ILogicState> FGameStateMachine::GetGameLogicState() const {
	return _logicState.ToSharedRef();
}

void FGameStateMachine::GoToState(const TSharedRef<FAbstractLogicState>& InNewState) {
	if (_logicState.IsValid()) {
		_logicState->OnExit();
	}

	_logicState = InNewState;
	InNewState->OwnerStateMachine = this->AsShared();
	OnLogicStateChanged.Broadcast(InNewState);

	InNewState->OnEnter();
}