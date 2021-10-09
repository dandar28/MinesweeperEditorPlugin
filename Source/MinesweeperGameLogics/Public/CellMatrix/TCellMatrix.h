#pragma once

#include "CoreMinimal.h"

#include "ICellMatrix.h"

/**
 * \brief - Templated class which can hold a concrete matrix of cells of any type.
 */
template <typename CellType>
class MINESWEEPERGAMELOGICS_API TCellMatrix
	: public ICellMatrix<CellType> {
public:
	virtual ~TCellMatrix() = default;

	/**
	 * \brief - Construct this matrix with a certain size of Width x Height.
	 * \param[in] InWidth - Width of the matrix to be built.
	 * \param[in] InHeight - Height of the matrix to be build.
	 */
	TCellMatrix(int InWidth, int InHeight) {
		_matrixSize = FIntPoint(InWidth, InHeight);

		_matrixData.SetNum(InWidth);
		for (int i = 0; i < InWidth; i++) {
			_matrixData[i].SetNum(InHeight);
		}
	}

	/**
	 * \brief - Get a cell from its coordinates.
	 * \param[in] InCoordinates - Coordinates of the cell to be returned.
	 * \return A const reference of a cell of CellType template argument.
	 */
	const CellType& Get(const FIntPoint& InCoordinates) const {
		return _matrixData[InCoordinates.X][InCoordinates.Y];
	}

	/**
	 * \brief - Get a cell from its coordinates.
	 * \param[in] InCoordinates - Coordinates of the cell to be returned.
	 * \return A reference of a cell of CellType template argument.
	 */
	CellType& Get(const FIntPoint& InCoordinates) {
		return _matrixData[InCoordinates.X][InCoordinates.Y];
	}

	/**
	 * \brief - Check the existence of a cell from its coordinates.
	 * \param[in] InCoordinates - Coordinates of the cell to be checked.
	 * \return True if the matrix has a cell for the input coordinates.
	 */
	bool Has(const FIntPoint& InCoordinates) const {
		return InCoordinates.X >= 0 && InCoordinates.Y >= 0 && InCoordinates.X < _matrixSize.X && InCoordinates.Y < _matrixSize.Y;
	}

	/**
	 * \return The size of the built matrix owned by this class.
	 */
	virtual FIntPoint GetSize() const { return _matrixSize; }

protected:
	/**
	 * \brief - Bidimensional array representing the matrix data and made through two nested arrays.
	 */
	TArray<TArray<CellType>> _matrixData;

	/**
	 * \brief - Size of the built matrix.
	 */
	FIntPoint _matrixSize;
};