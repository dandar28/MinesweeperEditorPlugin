#pragma once

#include "CoreMinimal.h"

/**
 * \brief - Interface holding a generic game data state
 */
template <class TGameState>
class TGameStateHolder {
public:
	TSharedRef<TGameState> GetGameDataState() const {
		return _gameDataState.ToSharedRef();
	}

protected:
	TSharedPtr<TGameState> _gameDataState;
};