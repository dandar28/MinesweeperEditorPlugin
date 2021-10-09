#pragma once

#include "CoreMinimal.h"

#include "ICellMatrix.h"

template <typename CellType>
struct MINESWEEPERGAMELOGICS_API TCellMatrix : public ICellMatrix<CellType> {
	virtual ~TCellMatrix() = default;

	TCellMatrix(int InWidth, int InHeight) {
		_matrixSize = FIntPoint(InWidth, InHeight);

		_matrixData.SetNum(InWidth);
		for (int i = 0; i < InWidth; i++) {
			_matrixData[i].SetNum(InHeight);
		}
	}

	const CellType& Get(const FIntPoint& InCoordinates) const {
		return _matrixData[InCoordinates.X][InCoordinates.Y];
	}

	CellType& Get(const FIntPoint& InCoordinates) {
		return _matrixData[InCoordinates.X][InCoordinates.Y];
	}

	bool Has(const FIntPoint& InCoordinates) const {
		return InCoordinates.X >= 0 && InCoordinates.Y >= 0 && InCoordinates.X < _matrixSize.X && InCoordinates.Y < _matrixSize.Y;
	}

	virtual FIntPoint GetSize() const { return _matrixSize; }

protected:
	TArray<TArray<CellType>> _matrixData;
	FIntPoint _matrixSize;
};