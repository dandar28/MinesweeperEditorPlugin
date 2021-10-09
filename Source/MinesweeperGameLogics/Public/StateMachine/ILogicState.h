#pragma once

#include "CoreMinimal.h"

/**
 * \brief - Interface for logic states that can handle state transition logics.
 */
class MINESWEEPERGAMELOGICS_API ILogicState {
public:
	virtual void OnEnter() = 0;
	virtual void OnExit() = 0;
};