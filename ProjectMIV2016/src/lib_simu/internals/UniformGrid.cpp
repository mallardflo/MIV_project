
#include "UniformGrid.h"
#include "MeshContainer.h"

UniformGrid::UniformGrid(MeshContainer& mesh) {

	//we use a version using lists, to the copy it to vectors for fast allocation and free
	std::vector<Cell_List> tmp_grid;

	//considering the bounding box
	_grid_box = mesh.bounded_box;
	_projection_box = _grid_box;
	_grid_box.scale_increase(0.02f);			//2% scale to be sure to have all the point inside the grid even with numerical imprecisions
	_grid_box.increase(Maths::Vector3(0.01f, 0.01f, 0.01f));
	_grid_origin = _grid_box.getMinimum();

	//compute the size of the grid.
	//we want a maximum of NB_VERTEX_PER_CELL per cell
	//we assume a uniform repartition of vertices into the bounding box of the mesh
	float vert_density = (float)mesh.nb_vertices/_grid_box.volume();
	//volume of a cell so that it contains NB_VERTEX_PER_CELL vertices (assuming a uniform repartition)
	float cell_volume = (float)NB_VERTEX_PER_CELL/vert_density;

	//assumnig first cubic cells
	float cell_side = pow((double)cell_volume, 1.0/3.0);

	//compute the number of cell in each axis with the cubic cells
	Maths::Vector3 box_size = _grid_box.getSize();
	_grid_size_x = (box_size.x/cell_side) + 1;
	_grid_size_y = (box_size.y/cell_side) + 1;
	_grid_size_z = (box_size.z/cell_side) + 1;
	if (_grid_size_x > 100) _grid_size_x = 100;
	if (_grid_size_y > 100) _grid_size_y = 100;
	if (_grid_size_z > 100) _grid_size_z = 100;
	_grid_size_yx = _grid_size_x*_grid_size_y;

	//compute the true size of cells to fit exactly the grid box
	_grid_cell_size.x = box_size.x/(float)_grid_size_x;
	_grid_cell_size.y = box_size.y/(float)_grid_size_y;
	_grid_cell_size.z = box_size.z/(float)_grid_size_z;

	_grid_nb_cells = _grid_size_x*_grid_size_y*_grid_size_z;

	//create the array using the number of vertices
	tmp_grid.resize(_grid_nb_cells);

	//loop over all the vertice to put them into a cell
	for (int i = 0 ; i < mesh.nb_vertices ; i++) {
		int i_cell = getCellFromPoint(mesh.vertices[i]);
		tmp_grid[i_cell].vals.push_back(i);
	}

	//copy the grid into the final vector
	_grid.clear();
	_grid.resize(_grid_nb_cells);
	std::list<int>::iterator mit;

	for (int i = 0 ; i < _grid_nb_cells ; i++) {

		Cell_List& vtn = tmp_grid[i];
		Cell& vn = _grid[i];
		int h = 0;
		vn.vals.resize(vtn.vals.size());

		for (mit = vtn.vals.begin() ; mit != vtn.vals.end() ; mit++) {
			vn.vals[h++] = (*mit);
		}
	}

	computeVoidCellNonVoidNearestCell();

}

std::vector<Val>& UniformGrid::getValsInCell(Maths::Vector3& p) {

	//first, project the point on the bounding box of the mesh (and not on the grid_box)
	Maths::Vector3 pt = p;
	_projection_box.projectPoint(pt);

	//get cell id from position
	int c_id = getCellFromPoint(pt);

	//if the cell is empty, jump to the nearest non void cell
	c_id = _grid[c_id].nearest_non_void;

	return _grid[c_id].vals;

}

void UniformGrid::computeVoidCellNonVoidNearestCell() 
{
	_neighborhoodIds = new CellId[6];
	_cellToCheck = new CellId[_grid_nb_cells]; 
	_trueValue = 1;
	_doneInThisCall = new int[_grid_nb_cells]; 
	memset(_doneInThisCall, 0, sizeof(int) * _grid_nb_cells);

	//std::vector<Distance> distanceToNonVoidNearestCell = std::vector<int>(std::vector<int>::size_type(_grid_nb_cells), _grid_nb_cells + 1); // < _grid_nb_cells + 1 == déjà calculé
	CellId currentCell = 0;
	CellId nearestNonVoidCell = 0;
	while(currentCell < _grid_nb_cells )
	{
		nearestNonVoidCell = getNonVoidNearestCell(currentCell);
		_grid[currentCell].nearest_non_void = nearestNonVoidCell;
		
		++currentCell;
		_trueValue++;
	}

	delete[] _neighborhoodIds;
	delete[] _cellToCheck;
	delete[] _doneInThisCall;
}



void UniformGrid::getNeighborhood( CellId cell_id, CellId* neighborhoodIds )
{
	neighborhoodIds[0]= cell_id - 1;
	neighborhoodIds[1]= cell_id + 1;
	neighborhoodIds[2]= cell_id - _grid_size_x;
	neighborhoodIds[3]= cell_id + _grid_size_x;
	neighborhoodIds[4]= cell_id - _grid_size_yx;
	neighborhoodIds[5]= cell_id + _grid_size_yx;
}

CellId UniformGrid::getNonVoidNearestCell( CellId cell )
{
	// if the cell is non void we return it immediately and avoid computing
	if ( _grid[cell].vals.size() > 0 )
	{
		return cell;
	}

	// cell which are at the same distance from the cell given in parameter.
	// if a non void cell is not in this list, we will get the next list of cell with a distance +1 from the cell.
	
	unsigned int numberOfCellInTab = 0;		// number of valid CellId in cellToCheck
	unsigned int cellIt = 0;				// cellToCheck[cellIt] -> the next Cell to check
	unsigned int nextCellIt = 0;			// the iterator from where the distance is not the same anymore (next have distance +1)
	
	_cellToCheck[numberOfCellInTab] = cell;
	++numberOfCellInTab;
	_doneInThisCall[cellIt] = _trueValue;

	
	
	while(nextCellIt!=numberOfCellInTab)
	{
		cellIt = nextCellIt;
		nextCellIt = numberOfCellInTab;
		for(unsigned int it = cellIt; it!=nextCellIt; ++it)
		{
			if ( _grid[_cellToCheck[it]].vals.size() > 0 ) // we found a non void, so we return it
			{
				unsigned int res = _cellToCheck[it];
				return res;
			}
			else
			{
				getNeighborhood(_cellToCheck[it], _neighborhoodIds);
				for(int neighborhoodIt = 0; neighborhoodIt!=6; ++neighborhoodIt)
				{
					if (_neighborhoodIds[neighborhoodIt]<_grid_nb_cells	&& 
						_neighborhoodIds[neighborhoodIt]>-1				&& 
						_doneInThisCall[_neighborhoodIds[neighborhoodIt]]!=_trueValue) // we take only available and not made neighborhood
					{
						_cellToCheck[numberOfCellInTab] = _neighborhoodIds[neighborhoodIt];
						++numberOfCellInTab;
						_doneInThisCall[_neighborhoodIds[neighborhoodIt]] = _trueValue;
					}
				}
			}
		}
	}

	return CellId(-1);
}

