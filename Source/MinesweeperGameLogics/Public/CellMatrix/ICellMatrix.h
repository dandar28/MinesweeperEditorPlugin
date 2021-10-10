#pragma once

#include "CoreMinimal.h"

/**
 * \brief - Interface with methods for a container of cells into a 2d matrix space.
 */
template <typename CellType>
struct MINESWEEPERGAMELOGICS_API ICellMatrix {
	virtual const CellType& Get(const FIntPoint& InCoordinates) const = 0;
	virtual CellType& Get(const FIntPoint& InCoordinates) = 0;
	virtual bool Has(const FIntPoint& InCoordinates) const = 0;
	virtual FIntPoint GetSize() const = 0;

	/**
	 * \return The total number of cells thorugh the matrix size, representing also the area.
	 */
	virtual int GetNumberOfCells() const {
		const FIntPoint MatrixSize = GetSize();
		return MatrixSize.X * MatrixSize.Y;
	}
};