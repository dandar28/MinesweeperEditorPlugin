#pragma once

#include "Minesweeper/IMinesweeperGameLogicState.h"

/**
 * \brief - Logic State that handles the actual playing of minesweeper.
 */
class MINESWEEPERGAMELOGICS_API FPlayingLogicState
	: public IMinesweeperGameLogicState {
public:
	virtual ~FPlayingLogicState() = default;

	void FlagOnCell(const FMinesweeperCellCoordinate& InCoordinates) override;
	void SweepOnCell(const FMinesweeperCellCoordinate& InCoordinates) override;

private:
	/**
	 * \brief - Reveal adjacent empty cells recursively from certain cell coordinates.
	 * \param[in] InCoordinates - Target coordinates around which trying to reveal adjacent empty cells.
	 */
	void _revealAdjacents(const FMinesweeperCellCoordinate& InCoordinates);
};