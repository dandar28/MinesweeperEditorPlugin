#pragma once

#include "CoreMinimal.h"

/**
 * \brief - Templated class that contains a game data state within it.
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