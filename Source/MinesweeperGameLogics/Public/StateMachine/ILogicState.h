#pragma once

#include "CoreMinimal.h"

class MINESWEEPERGAMELOGICS_API ILogicState {
public:
	virtual void OnEnter() = 0;
	virtual void OnExit() = 0;
};