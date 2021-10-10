#pragma once

#include "CoreMinimal.h"

#include "FAbstractLogicState.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLogicStateChanged, TSharedRef<FAbstractLogicState>);

/**
 * \brief - Concrete class for handling simple finite state machine transitions.
 */
class MINESWEEPERGAMELOGICS_API FGameStateMachine
	: public TSharedFromThis<FGameStateMachine> {
public:
	/**
	 * \return The current game logic state.
	 */
	TSharedRef<ILogicState> GetGameLogicState() const;

	/**
	 * \brief - Performs a transition to a new state.
	 * \param[in] InNewState - New state to transit for.
	 */
	void GoToState(const TSharedRef<FAbstractLogicState>& InNewState);

	/**
	 * \return The current game logic state statically casted to TConcreteLogicState template parameter.
	 */
	template <class TConcreteLogicState>
	TSharedRef<TConcreteLogicState> GetGameLogicStateAs() const {
		return StaticCastSharedRef<TConcreteLogicState>(_logicState.ToSharedRef());
	}

	/**
	 * \brief - Performs a transition to a new state instanced from type TConcreteLogicState template parameter.
	 */
	template <class TConcreteLogicState>
	void GoToState() {
		return GoToState(MakeShared<TConcreteLogicState>());
	}

	/**
	 * \brief - Event delegate that is triggered whenever a state transition happens.
	 */
	FOnLogicStateChanged OnLogicStateChanged;

protected:
	/**
	 * \brief - Current game logic state owned by the state machine.
	 */
	TSharedPtr<ILogicState> _logicState;
};