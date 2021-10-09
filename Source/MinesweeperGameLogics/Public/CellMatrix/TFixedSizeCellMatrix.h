#pragma once

#include "CoreMinimal.h"

#include "ICellMatrix.h"

/**
 * \brief - Templated class for fixed size cell matrices.
 */
template <int NumRows, int NumCols, typename CellType>
class MINESWEEPERGAMELOGICS_API TFixedSizeCellMatrix
	: public ICellMatrix<CellType> {
public:
	const CellType& Get(const FIntPoint& InCoordinates) const {
		return _matrixData[InCoordinates.X][InCoordinates.Y];
	}

	CellType& Get(const FIntPoint& InCoordinates) {
		return _matrixData[InCoordinates.X][InCoordinates.Y];
	}

	bool Has(const FIntPoint& InCoordinates) const {
		return InCoordinates.X >= 0 && InCoordinates.Y >= 0 && InCoordinates.X < NumRows && InCoordinates.Y < NumCols;
	}

protected:
	CellType _matrixData[NumCols][NumRows];
};