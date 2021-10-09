#pragma once

#include "CoreMinimal.h"

/**
 * \brief - Templated class that contains a game data state within it.
 */
template <class TGameState>
class TGameStateHolder {
public:
	/**
	 * \return The game data state held by this class.
	 */
	TSharedRef<TGameState> GetGameDataState() const {
		return _gameDataState.ToSharedRef();
	}

protected:
	/**
	 * \brief - Game state held by this class.
	 */
	TSharedPtr<TGameState> _gameDataState;
};