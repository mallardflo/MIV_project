
#ifndef UNIFORM_GRID_H
#define UNIFORM_GRID_H

#include "AllMath.h"
#include <list>
#include <vector>

typedef int CellId;					/*!< Id a Cell in the grid */
typedef int Val;					/*!< type of a single value of a Cell */
typedef unsigned int Distance;		/*!< distance of cell between each other */

struct Cell_List {
	CellId nearest_non_void;
	std::list<int> vals;	
};

struct Cell {
	CellId nearest_non_void;			/*!< used only when the Cell is empty else point to itself */
	std::vector<int> vals;				/*!< if size=0 => the Cell is empty */
};

class MeshContainer;

/*! \class UniformGrid
 * \brief Uniform static grid, each cell contain an array of element of type T (T is "int" for the moment, this class could have been templated).
 */
class UniformGrid {

private:

	void freeMemory();

protected:

	Maths::AxisAlignedBox _grid_box;					/*!< The boundaries of the box */
	Maths::AxisAlignedBox _projection_box;			/*!< Nearly same as box, but slightly strecth to avoid numerical imprecision when projecting on box */

	int _grid_size_x;								/*!< Number of cells in the x-axis direction */
	int _grid_size_y;								/*!< Number of cells in the y-axis direction */
	int _grid_size_z;								/*!< Number of cells in the z-axis direction */
	int _grid_size_yx;								/*!< Number of cells in the xy plane = x*y*/

	Maths::Vector3 _grid_cell_size;					/*!< Spatial occupation of a cell */
	Maths::Vector3 _grid_origin;						/*!< Minimum corner position */
	std::vector<Cell> _grid;							/*!< Uniform 3D grid. each cell contain a list of vertex indices */
	int _grid_nb_cells;								/*!< Total number of cells into the grid (equals to grid_size_x*grid_size_y*grid_size_z) */

	/*!
	 * \brief giving a 3D pos, return the grid cell id
	 */
	inline int getCellFromPoint(Maths::Vector3& p) { 
		int x = int((p.x - _grid_origin.x)/_grid_cell_size.x);
		int y = int((p.y - _grid_origin.y)/_grid_cell_size.y);
		int z = int((p.z - _grid_origin.z)/_grid_cell_size.z);
		return (z*_grid_size_yx + y*_grid_size_x + x);
	}

public:

	UniformGrid(MeshContainer& mesh);

	/*!
	 * \brief giving a 3D pos, return the list of entries of the corresponding cell
	 * If the corresponding cell is empty, the nearest non empty cell is chosen.
	 */
	std::vector<Val>& getValsInCell(Maths::Vector3& p);

	/*!
	 * \brief Compute for each cell the nearest non void cell
	 */
	void computeVoidCellNonVoidNearestCell();


private:
	/*!
	 *  \param	cell the id of the cell in the grid 
	 *  \param	neighbourhoodIds out  the result vector of neighborhood, MUST BE INITIALIZED before the call
	 *			of the function, and be able to store 6 CellId.
	 *			A CellId valid value is positive and < _grid_nb_cells, else the cell is out of the grid.
	 */
	void getNeighborhood(CellId cell, CellId* neighborhoodIds);

	/*!
	 *  \param cell the cell from wich we are looking the minimal distance to a non-void cell
	 *
	 */
	CellId getNonVoidNearestCell(CellId cell);

/*
	CellId*		_prev;				//!<  _prev[x] -> the previous cell of y in the path from x to y
	Distance*	_dist;				//!<  _dist[x + y*_grid_nb_cells] -> the distance between y and x */
	int*		_doneInThisCall;	//!<  true if this cell bas been done in this call of getMinimalDistanceToNonVoidNearestCell (avoid to return to the caller)
	CellId*		_neighborhoodIds;	//!<  usefull array to neighborhood request.
	CellId*		_cellToCheck;		//!<  list of cell to check when looking for non-void cell

	int			_trueValue;			//!<  int who mean true
};

#endif