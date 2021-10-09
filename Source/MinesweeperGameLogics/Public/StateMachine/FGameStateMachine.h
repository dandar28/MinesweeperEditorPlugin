#pragma once

#include "CoreMinimal.h"

#include "FAbstractLogicState.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLogicStateChanged, TSharedRef<FAbstractLogicState>);

/**
 * \brief - Interface holding a generic game logic state machine
 */
class MINESWEEPERGAMELOGICS_API FGameStateMachine : public TSharedFromThis<FGameStateMachine> {
public:
	TSharedRef<ILogicState> GetGameLogicState() const;

	void GoToState(const TSharedRef<FAbstractLogicState>& InNewState);

	template <class TConcreteLogicState>
	TSharedRef<TConcreteLogicState> GetGameLogicStateAs() const {
		return StaticCastSharedRef<TConcreteLogicState>(_logicState.ToSharedRef());
	}

	template <class TConcreteLogicState>
	void GoToState() {
		return GoToState(MakeShared<TConcreteLogicState>());
	}

	FOnLogicStateChanged OnLogicStateChanged;

protected:
	TSharedPtr<ILogicState> _logicState;
};