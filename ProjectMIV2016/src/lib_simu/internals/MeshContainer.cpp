
#include "MeshContainer.h"
#include "StringExtension.h"

#include "AllMath.h"

#include <fstream>
#include <iostream>
#include <list>

#include "Utils.h"

MeshContainer::MeshContainer(MestType mesh_type) {

	initialize();

	initializeMeshType(mesh_type);

}

void MeshContainer::initializeMeshType(MestType mesh_type) {

	bool mesh_type_valid = false;

	//depending on mesh type
	int h = 0;

	//tetrahedron
	if (mesh_type == MESH_TYPE_TETRAHEDRON) {
		nb_vert_per_elem = 4;
		nb_face_per_elem = 4;
		nb_vert_per_face = 3;
		nb_indices_per_face = 3;
		faces_vertices_indices = new int[nb_face_per_elem*nb_indices_per_face];
		faces_vertices_indices[h++] = 0; faces_vertices_indices[h++] = 3; faces_vertices_indices[h++] = 2;
		faces_vertices_indices[h++] = 2; faces_vertices_indices[h++] = 3; faces_vertices_indices[h++] = 1;
		faces_vertices_indices[h++] = 1; faces_vertices_indices[h++] = 3; faces_vertices_indices[h++] = 0;
		faces_vertices_indices[h++] = 1; faces_vertices_indices[h++] = 0; faces_vertices_indices[h++] = 2;
		mesh_type_valid = true;
	}

	if (mesh_type == MESH_TYPE_TRIANGLES) {
		nb_vert_per_elem = 3;
		nb_face_per_elem = 3;
		nb_vert_per_face = 2;
		nb_indices_per_face = 2;
		faces_vertices_indices = new int[nb_face_per_elem*nb_indices_per_face];
		faces_vertices_indices[h++] = 0; faces_vertices_indices[h++] = 1;
		faces_vertices_indices[h++] = 1; faces_vertices_indices[h++] = 2;
		faces_vertices_indices[h++] = 2; faces_vertices_indices[h++] = 0;
		mesh_type_valid = true;
	}

	if (!mesh_type_valid) Message::error("The mesh type is not valid");


}

void MeshContainer::initialize() {
	vert_indices_valid = false;
	child_mesh = false;
	nb_indices = 0;
	nb_surface_indices = 0;
	nb_references = 0;
	indices = NULL;
	surface_indices = NULL;
	uniform_grid = NULL;
	elements_neighbors = NULL;
	faces_vertices_indices = NULL;
	vertices = NULL;
}

MeshContainer::MeshContainer(const MeshContainer& src) {
	copy(src);
}
MeshContainer::~MeshContainer() {
	free_memory();
}

bool MeshContainer::loadFromGmshFile(std::string file_name) {

	//open the file (no check, because this private function is called only after checks about the file)
	std::ifstream f;
	f.open(file_name.c_str());	

	//temporary storing the indice buffer in a list fashion
	std::list<int> v_ind;
	std::list<int> s_ind;
	nb_vertices = 0;

	//defines useful variables for further reding
	const int MAX_LENGTH = 500;
    char line[MAX_LENGTH];
	std::string sline;			//current line
	std::vector<std::string> v_string;	//split line

	//first reach the node line, and get the number of nodes...
	while (f.getline(line, MAX_LENGTH)) {
		//turn the line into a string
		sline = std::string(line);
		if (sline.compare("$Nodes") == 0) {
			f.getline(line, MAX_LENGTH);
			sline = std::string(line);
			nb_vertices = StringExtension::stringToInt(sline);
			break;
		}
	}

	if (nb_vertices < 1) return false;
	//resize the vertice array !
	delete[] vertices;
	vertices = new Maths::Vector3[nb_vertices];

	//get the nodes coordinates
	float x, y, z;
	for(int i = 0 ; i < nb_vertices ; i++) {

		//turn the line into a string
		f.getline(line, MAX_LENGTH);
		sline = std::string(line);

		//split the line with space!!
		StringExtension::stringSplit(v_string, sline, " ");
		
		//ignore empty line
		if (v_string.size() == 0) { i--; continue; }

		//store the coordinates
		x = StringExtension::stringToFloat(v_string[1]);
		y = StringExtension::stringToFloat(v_string[2]);
		z = StringExtension::stringToFloat(v_string[3]);

		vertices[i] = Maths::Vector3(x, y, z);

	}

	//ok, now get the elements (the indices)
	int nb_elems = 0;
	while (f.getline(line, MAX_LENGTH)) {
		//turn the line into a string
		sline = std::string(line);
		if (sline.compare("$Elements") == 0) {
			f.getline(line, MAX_LENGTH);
			sline = std::string(line);
			nb_elems = StringExtension::stringToInt(sline);
			break;
		}
	}

	//loop over all the elements
	int type;
	int nb_tags;
	for(int i = 0 ; i < nb_elems ; i++) {

		//turn the line into a string
		f.getline(line, MAX_LENGTH);
		sline = std::string(line);

		//split the line with space!!
		StringExtension::stringSplit(v_string, sline, " ");

		//get the type of the element and the number of tags
		type = StringExtension::stringToInt(v_string[1]);
		nb_tags = StringExtension::stringToInt(v_string[2]);

		//dependeing on the type...
		//triangle
		if (type == 2){
			for(int h = 0 ; h < 3 ; h++) s_ind.push_back(StringExtension::stringToInt(v_string[h+nb_tags+3]) - 1);			
		}

		//tetrahedron
		if (type == 4){
			for(int h = 0 ; h < nb_vert_per_elem ; h++) v_ind.push_back(StringExtension::stringToInt(v_string[h+nb_tags+3]) - 1);
		}

		//unsupported type we may support...
		if (type == 3 || (type >= 5 && type <= 14) || type > 16) {
			Message::message("warning loading gmesh file : unsupported element type (" + StringExtension::intToString(type) + ")");
		}

	}

	//we've finished here for file reading
	f.close();

	//copy all the lists into the vector buffers...
	int i = 0;
	std::list<int>::iterator iit;

	nb_indices = v_ind.size();
	indices = new int[nb_indices];
	for (iit = v_ind.begin(); iit != v_ind.end() ; iit++) indices[i++] = (*iit);

	//surface no longer loaded...
	//i = 0;
	//indices_surface.resize(s_ind.size());
	//for (iit = s_ind.begin(); iit != s_ind.end() ; iit++) indices_surface[i++] = (*iit);

	//ok, great !!
	return true;

}

bool MeshContainer::loadFromOffFile(std::string file_name) {

	//open the file (no check, because this private function is called only after checks about the file)
	std::ifstream f;
	f.open(file_name.c_str());	

	//defines useful variables for further reding
	const int MAX_LENGTH = 500;
    char line[MAX_LENGTH];
	std::string sline;			//current line
	std::vector<std::string> v_string;	//split line

	//header
	f.getline(line, MAX_LENGTH);
	sline = std::string(line);
	if (sline.compare("OFF") != 0) {
		Message::error("The off file is not valid.");
		return false;
	}
	f.getline(line, MAX_LENGTH);
	sline = std::string(line);
	StringExtension::stringSplit(v_string, sline, " ");
	nb_vertices = StringExtension::stringToInt(v_string[0]);
	if (nb_vertices < 1) return false;	
	delete[] vertices;
	vertices = new Maths::Vector3[nb_vertices];

	//load vertices
	for (int i = 0 ; i < nb_vertices ; i++) {
		f.getline(line, MAX_LENGTH);
		sline = std::string(line);
		StringExtension::stringSplit(v_string, sline, " ");
		vertices[i].x = StringExtension::stringToFloat(v_string[0]);
		vertices[i].y = StringExtension::stringToFloat(v_string[1]);
		vertices[i].z = StringExtension::stringToFloat(v_string[2]);
	}
	f.close();

	//load elements
	std::string fn = StringExtension::getFileNameFromPath(file_name, false);
	fn = StringExtension::getPathFromFileName(file_name) + fn + ".ele";
	f.open(fn.c_str());

	f.getline(line, MAX_LENGTH);
	sline = std::string(line);
	StringExtension::stringSplit(v_string, sline, " ");
	nb_elements = StringExtension::stringToInt(v_string[0]);
	nb_indices = nb_elements*nb_vert_per_elem;
	indices = new int[nb_indices];
	int h = 0;
	for (int i = 0 ; i < nb_elements ; i++) {
		f.getline(line, MAX_LENGTH);
		sline = std::string(line);
		StringExtension::stringSplit(v_string, sline, " ");
		indices[h++] = int(StringExtension::stringToFloat(v_string[1]));
		indices[h++] = int(StringExtension::stringToFloat(v_string[2]));
		indices[h++] = int(StringExtension::stringToFloat(v_string[3]));
		indices[h++] = int(StringExtension::stringToFloat(v_string[4]));
	}

	f.close();

	return true;

}

/**
 * Load mesh from an OBJ file
 *
 */
bool MeshContainer::loadFromObjFile(std::string file_name) {

	//open the file
	std::ifstream f;
	f.open(file_name.c_str());	

	//ok, first, the vertex list !
	std::list<Maths::Vector3> verts;
	std::list<Maths::Vector3> norms;
	std::list<int> v_ind;
	std::list<int> n_ind;
	//verts.reserve(1000);
	//norms.reserve(1000);
	//v_ind.reserve(3000);
	//n_ind.reserve(3000);

	//defines useful variables for further reding
	const int MAX_LENGTH = 400;
    char line[MAX_LENGTH];
	std::string sline;
	std::vector<std::string> v_string;
	std::vector<std::string> split;
	float v1, v2, v3;	

	//loop over the lines of the file
	while (f.getline(line, MAX_LENGTH)) {

		//turn the line into a string
		sline = std::string(line);

		//split the line !!
		StringExtension::stringSplit(v_string, sline, " ");
		
		//ignore empty line
		if (v_string.size() == 0) continue;

		//depending on the first character of the line, switch
		if (v_string[0].compare("v") == 0) {
			//a vertex, add to the list !!
			v1 = StringExtension::stringToFloat(v_string[1]);
			v2 = StringExtension::stringToFloat(v_string[2]);
			v3 = StringExtension::stringToFloat(v_string[3]);
			verts.push_back(Maths::Vector3(v1, v2, v3));
		}

		if (v_string[0].compare("vn") == 0) {
			//a normal, add to the list !!
			v1 = StringExtension::stringToFloat(v_string[1]);
			v2 = StringExtension::stringToFloat(v_string[2]);
			v3 = StringExtension::stringToFloat(v_string[3]);
			norms.push_back(Maths::Vector3(v1, v2, v3));
		}

		if (v_string[0].compare("f") == 0) {
			bool normals = false;
			//read the first two vertices
			int n1, n2, n3;
			StringExtension::stringSplit(split, v_string[1], "/");
			v1 = StringExtension::stringToFloat(split[0]);
			if (split.size() == 2) { normals = true; n1 = int(StringExtension::stringToFloat(split[1])); }
			StringExtension::stringSplit(split, v_string[2], "/");
			v2 = StringExtension::stringToFloat(split[0]);
			if (normals) { n2 = int(StringExtension::stringToFloat(split[1])); }
			//each next vertex form a new triangle v1-v2-v3
			for (unsigned int i = 3 ; i < v_string.size() ; i++) {
				StringExtension::stringSplit(split, v_string[i], "/");
				v3 = StringExtension::stringToFloat(split[0]);
				if (normals) n3 = int(StringExtension::stringToFloat(split[1]));
				//add the triangle
				v_ind.push_back(int(v1 - 1)); v_ind.push_back(int(v2 - 1)); v_ind.push_back(int(v3 - 1));
				if (normals) { n_ind.push_back(n1 - 1); n_ind.push_back(n2 - 1); n_ind.push_back(n3 - 1); }
				v2 = v3; n2 = n3;
			}
		}
	}

	f.close();

	//copy all the lists into the vector buffers...
	int i = 0;
	std::list<Maths::Vector3>::iterator it;
	nb_vertices = verts.size();
	vertices = new Maths::Vector3[nb_vertices];
	for (it = verts.begin(); it != verts.end() ; it++) vertices[i++] = (*it);
	i = 0;
	//normals.resize(norms.size());
	//for (it = norms.begin(); it != norms.end() ; it++) normals[i++] = (*it);
	//i = 0;
	nb_indices = v_ind.size();
	std::list<int>::iterator iit;
	indices = new int[nb_indices];
	for (iit = v_ind.begin(); iit != v_ind.end() ; iit++) indices[i++] = (*iit);
	i = 0;
	//n_index.resize(n_ind.size());
	//for (iit = n_ind.begin(); iit != n_ind.end() ; iit++) n_index[i++] = (*iit);

	//ok, great !!
	return true;

}

void MeshContainer::getVertexIndiceArray() {

	//useless to do the job again
	if (vert_indices_valid) return;

	//we use a version using lists, to the copy it to vectors for fast aloocation and free
	std::vector<VertexIndicesList> tmp_indices;

	//create the array using the number of vertices
	tmp_indices.clear();
	tmp_indices.resize(nb_vertices);

	//loop over all the indices 
	int h = 0;
	for (int i = 0 ; i < nb_indices; i++) {
		//add the entry to the vertex indice array
		tmp_indices[indices[i]].indices.push_back(h);
		h++;
	}

	//copy the list version into the vert_indices variable
	vert_indices.clear();
	vert_indices.resize(nb_vertices);
	std::list<int>::iterator mit;

	for (int i = 0 ; i < nb_vertices ; i++) {

		VertexIndicesList& vtn = tmp_indices[i];
		VertexIndices& vn = vert_indices[i];
		int h = 0;
		vn.indices.resize(vtn.indices.size());

		for (mit = vtn.indices.begin() ; mit != vtn.indices.end() ; mit++) {
			vn.indices[h++] = (*mit);
		}
	}

	vert_indices_valid = true;

}

void MeshContainer::computeVertexGrid() {

	//delete the old grid
	delete uniform_grid;

	//create the uniform grid
	uniform_grid = new UniformGrid(*this);

}

void MeshContainer::initElements() {

	//allocate the elements member
	nb_elements = nb_indices/nb_vert_per_elem;
	delete[] elements_neighbors;
	elements_neighbors = new MeshElement*[nb_elements*nb_face_per_elem];
	elements.clear();
	elements.resize(nb_elements);

	//set up markers to zero
	for (int i = 0 ; i < nb_elements ; i++) {
		elements[i].id = i;
		elements[i].marker = 0;
		elements[i].neighbors = &elements_neighbors[i*nb_face_per_elem];
	}

	//first marker is one 
	element_marker = 1;

}

void MeshContainer::computeElementsNeighbors() {

	//build all needed tables
	getVertexIndiceArray();

	const int elem_size = nb_vert_per_elem;

	std::vector<int>::iterator mit;

	int add_indice;
	int e = 0;
	
	element_marker++;

	//temporary allocation
	int* list1 = new int[nb_vert_per_elem];
	int* list2 = new int[nb_vert_per_elem];
	int* list3 = new int[nb_vert_per_face];
	int* common_list = new int[nb_vert_per_elem];
	const int max_neighbs = 1000;
	std::vector<MeshElement*> neighbs;
	neighbs.resize(max_neighbs);
	int nb_ne;

	//for each element
	for (int i = 0 ; i < nb_indices ; i += elem_size) {

		nb_ne = 0;

		//for each vertex of this element
		for (int h = 0 ; h < elem_size ; h++) {
			
			VertexIndices& vn = vert_indices[indices[i + h]];

			//we use the table of vetex indices to compute neighbors
			for (mit = vn.indices.begin() ; mit != vn.indices.end() ; mit++) {
				//using the indice of the vertex, we can deduce the neighbor element
				add_indice = (int)((*mit)/elem_size);
				if (elements[add_indice].marker != element_marker) {
					neighbs[nb_ne++] = &elements[add_indice];
					elements[add_indice].marker = element_marker;
				}
			}
		}

		//for each element in neighs, we keep only those which have at least 3 vertices (a face) in common
		//it remains at max 4 neighbors
		int nb_common;

		//reset neighbor information
		for (int f = 0 ; f < nb_face_per_elem ; f++) {
			elements[e].neighbors[f] = NULL;
		}

		//find "true" neighbors
		for (int n = 0 ; n < nb_ne ; n++) {

			nb_common = 0;

			//check the number of common point between e and neigbhor n
			for (int e1 = e*nb_vert_per_elem, h = 0 ; h < nb_vert_per_elem ; e1++, h++) list1[h] = indices[e1];
			for (int e1 = neighbs[n]->id*nb_vert_per_elem, h = 0 ; h < nb_vert_per_elem ; e1++, h++) list2[h] = indices[e1];
			for (int w = 0 ; w < nb_vert_per_elem ; w++) for (int z = 0 ; z < nb_vert_per_elem ; z++) if (list1[w] == list2[z]) { common_list[nb_common++] = list1[w]; }

			//we got one !
			if (nb_common == nb_vert_per_face) {
				//find the face id for the neigbhor
				int f_id = -1;			
				for (int f = 0 ; f < nb_face_per_elem ; f++) {
					nb_common = 0;	
					getElemFaceVerticesIndices(e, f, list3);
					for (int w = 0 ; w < nb_vert_per_face ; w++) for (int z = 0 ; z < nb_vert_per_face ; z++) if (common_list[w] == list3[z]) nb_common++;
					if (nb_common == nb_vert_per_face) {
						f_id = f;
						break;
					}
				}
				if (f_id != -1) elements[e].neighbors[f_id] = neighbs[n];
			}
		}

		//reset element marking
		element_marker++;

		e++;
	}

	//free memory
	delete[] list1;
	delete[] list2;
	delete[] list3;
	delete[] common_list;
}

void MeshContainer::computeVerticesNeighbors() {

	const int elem_size = nb_vert_per_elem;
	std::vector<int>::iterator mit;
	int indice, v2;

	//temporary allocation
	const int max_neighbs = 500;
	int* neighbs = new int[max_neighbs];
	int nb_ne;

	//get the super_index table
	getVertexIndiceArray();

	//reset the neigbhor table
	vert_neighbors.clear();
	vert_neighbors.resize(nb_vertices);

	//initialize marking
	for (int v = 0 ; v < nb_vertices ; v++) {
		vert_neighbors[v].marker = 0;
	}
	vertex_marker = 1;

	//for each vertex
	for (int v = 0 ; v < nb_vertices ; v++) {
		
		VertexIndices& vn = vert_indices[v];

		//a vertex is not a neigbor of himself
		vert_neighbors[v].marker = vertex_marker;
		nb_ne = 0;

		//we use the table of vetex indices to compute neighbors
		for (mit = vn.indices.begin() ; mit != vn.indices.end() ; mit++) {
			//using the indice of the vertex, we can deduce the neighbor element
			indice = (int)((*mit)/elem_size)*elem_size;

			//loop over the vertices of the element
			for (int e = indice ; e < indice + elem_size ; e++) {
				v2 = indices[e];
				//add them to the neighborhood of the vertex i
				if (vert_neighbors[v2].marker != vertex_marker) {
					neighbs[nb_ne++] = v2;
					vert_neighbors[v2].marker = vertex_marker;
				}
			}
		}

		//copy from the temporary table to the vertex neighbor
		vert_neighbors[v].neighbors.resize(nb_ne);
		for (int n = 0 ; n < nb_ne ; n++) {
			vert_neighbors[v].neighbors[n] = neighbs[n];
		}

		//ok for this vertex...
		vertex_marker++;
	}

	delete[] neighbs;
}

void MeshContainer::minimizeVerticesBandwidth() {

	//build all needed tables
	computeVerticesNeighbors();

	//iterators
	std::vector<int>::iterator mit;

	vertex_marker++;

	//the list of element candidates...
	int* vert_queue = new int[nb_vertices];
	int queue_head = 0;
	int nb_elem_queue = 0;

	//to start with, we look for the vertice that has the less neighbors
	int i_start = 0;
	int n_min = vert_neighbors[i_start].neighbors.size();
	int n;
	for (int i = 1 ; i < nb_vertices ; i++) {
		n = vert_neighbors[i].neighbors.size();
		if (n < n_min) {
			n_min = n;
			i_start = i;
		}
	}

	//initialize vertices queue
	vert_queue[nb_elem_queue++] = i_start;
	vert_neighbors[vert_queue[queue_head]].marker = vertex_marker;

	while (nb_elem_queue > queue_head) {

		std::vector<int> neighbors = vert_neighbors[vert_queue[queue_head]].neighbors;

		//for each neighbor of the element in head of the queue		
		for (mit = neighbors.begin() ; mit != neighbors.end() ; mit++) {
			
			int vn = (*mit);

			//check if not already in the queue
			if (vert_neighbors[vn].marker != vertex_marker) {

				//push them into the queue
				vert_queue[nb_elem_queue++] = vn;

				//mark it!
				vert_neighbors[vn].marker = vertex_marker;
			}

		}

		//increment head pointer
		queue_head++;

	}

	//we have the new ordering of the vertices, rebuild the array
	std::vector<int>::iterator iit;
	Maths::Vector3* vert_copy = new Maths::Vector3[nb_vertices];
	memcpy(vert_copy, vertices, nb_vertices*sizeof(Maths::Vector3));

	for (int v = 0 ; v < nb_vertices ; v++) {

		int id = vert_queue[v];

		//copy the right vertex
		vertices[v] = vert_copy[id];

		//update the indices array
		for (iit = vert_indices[id].indices.begin() ; iit != vert_indices[id].indices.end() ; iit++) {
			indices[(*iit)] = v;
		}

	}

	//free used memory
	delete[] vert_queue;
	delete[] vert_copy;

	vert_indices_valid = false;

}

void MeshContainer::minimizeElementsBandwidth() {

	//build all needed tables
	getVertexIndiceArray();
	computeElementsNeighbors();

	//iterators
	int mit;

	//the list of element candidates...
	int* elem_queue = new int[nb_elements];
	int queue_head = 0;
	int nb_elem_queue = 0;
	elem_queue[nb_elem_queue++] = 0;		//we start propagating the search from the first element...
	elements[elem_queue[queue_head]].marker = element_marker;

	while (nb_elem_queue > queue_head) {

		MeshElement** neighbors = elements[elem_queue[queue_head]].neighbors;

		//for each neighbor of the element in head of the queue		
		for (mit = 0 ; mit < nb_face_per_elem ; mit++) {
			
			if (neighbors[mit] == NULL) continue;

			MeshElement* me = &elements[neighbors[mit]->id];

			//check if not already in the queue
			if (me->marker != element_marker) {

				//push them into the queue
				elem_queue[nb_elem_queue++] = me->id;

				//mark it!
				me->marker = element_marker;
			}

		}

		//increment head pointer
		queue_head++;

	}

	//the table elem_queue contains the new ordering that we apply with and temporary index
	int* indices_copy = new int[nb_indices];
	memcpy(indices_copy, indices, nb_indices*sizeof(int));
	int isrc, idest;
	for (int e = 0 ; e < nb_elements ; e++) {
		isrc = elem_queue[e]*nb_vert_per_elem;
		idest = e*nb_vert_per_elem;
		//copy using the new table
		indices[idest + 0] = indices_copy[isrc + 0]; indices[idest + 1] = indices_copy[isrc + 1]; indices[idest + 2] = indices_copy[isrc + 2]; indices[idest + 3] = indices_copy[isrc + 3];
	}

	//free used memory
	delete[] elem_queue;
	delete[] indices_copy;

	vert_indices_valid = false;

}

bool MeshContainer::loadFromFile(std::string file_name, float scale, Maths::Vector3 offset, Maths::Matrix3 rotation) {

	bool fail = false;
	Timer timer;

	//check if the file exists
	std::ifstream f;
	f.open(file_name.c_str());	
	if (!f) {
		Message::error("error with file \"" + file_name + "\" : can't find the file, or can't read it!");
		return false;
	} else {
		f.close();
	}

	//get the type of the file
	std::string ext;
	bool has_extension = StringExtension::getFileExtension(ext, file_name);
	if (!has_extension) {
		Message::error("error with file \"" + file_name + "\" : can't determine the type of the file!");
		return false;
	}

	bool found = false;
	//find the good function depending on the extension of the file
	if (ext.compare("gmsh") == 0) {
		found = true;
		timer.restart();
		fail = !loadFromGmshFile(file_name);
		time_loading = timer.elapsedTime();
		if (!fail) {	
			//post-loading treatments
			timer.restart();
			removeUnusedVertices();
			time_removing_unused_vertices = timer.elapsedTime();
			Message::message("Removing unused vertex time : " + StringExtension::floatToString(time_removing_unused_vertices));	
			timer.restart();
			minimizeVerticesBandwidth();
			float t10 = timer.elapsedTime();
			Message::message("Reordering time : " + StringExtension::floatToString(t10));	
		}
	}

	if (ext.compare("obj") == 0) {
		found = true;
		timer.restart();
		fail = !loadFromObjFile(file_name);
		time_loading = timer.elapsedTime();
	}

	//off file
	if (ext.compare("off") == 0) {
		//check if we can find the associated .ele file
		std::string fn = StringExtension::getFileNameFromPath(file_name, false);
		fn = StringExtension::getPathFromFileName(file_name) + fn + ".ele";
		f.open(fn.c_str());	
		if (!f.good()) {
			Message::error("Can't find the associated element file \"" + fn + "\", or can't read it!");
			return false;
		} else {
			f.close();
		}
		found = true;
		timer.restart();
		fail = !loadFromOffFile(file_name);
		time_loading = timer.elapsedTime();
		if (!fail) {	
			//post-loading treatments
			timer.restart();
			removeUnusedVertices();
			time_removing_unused_vertices = timer.elapsedTime();
			Message::message("Removing unused vertex time : " + StringExtension::floatToString(time_removing_unused_vertices));	
			timer.restart();
			minimizeVerticesBandwidth();
			float t10 = timer.elapsedTime();
			Message::message("Reordering time : " + StringExtension::floatToString(t10));	
		}
	}

	//binary file
	if (ext.compare("mshdat") == 0) {
		found = true;
		timer.restart();
		fail = !loadFromBinaryFile(file_name);
		time_loading = timer.elapsedTime();
	}

	//error checking...
	if (!found) {
		Message::error("error with file \"" + file_name + "\" : extension " + ext + " is not supported yet!");
		return false;
	}
	if (fail) {
		Message::error("the loading of \"" + file_name + "\" failed!");
		return false;
	}

	//post treatments
	timer.restart();
	computeBoundedBox();
	computeVerticesNeighbors();
	initElements();
	computeElementsNeighbors();
	computeVertexGrid();

	float t10 = timer.elapsedTime();
	Message::message("Other mesh computations time : " + StringExtension::floatToString(t10));	

	//output stats
	Message::message("Loading time : " + StringExtension::floatToString(time_loading));	

	//scale, rotate and offset the model...
	for (int i = 0 ; i < nb_vertices ; i++) {
		vertices[i] = rotation*(vertices[i]*scale) + offset;
	}

	return !fail;

}

int MeshContainer::getVertexFromPosition(Maths::Vector3& pos) {

	//Timer t;
	//t.restart();

	//ok, look for the closest point in this cell
	std::vector<int>& vi = uniform_grid->getValsInCell(pos);

	if (vi.size() == 0) return -1;
	int i_min = vi[0];
	float d_min = float(pos.squaredDistance(vertices[vi[0]]));
	float d;

	for (unsigned int i = 1 ; i < vi.size() ; i++) {
		d = pos.squaredDistance(vertices[vi[i]]);
		if (d < d_min) {
			d_min = d;
			i_min = vi[i];
		}
	}

	//we're still not done. To avoid too much border effect, we look at the found vertiex neighbors
	if (vert_neighbors.size() == 0) return i_min;
	std::vector<int>& vi2 = vert_neighbors[i_min].neighbors;
	for (unsigned int i = 1 ; i < vi2.size() ; i++) {
		d = pos.squaredDistance(vertices[vi2[i]]);
		if (d < d_min) {
			d_min = d;
			i_min = vi2[i];
		}
	}

	//float f = t.elapsedTime();
	//Message::debug("found point in " + StringExtension::formatTime(f));

	return i_min;

}

void MeshContainer::removeUnusedVertices() {

	std::vector<int>::iterator iit;
	std::list<Maths::Vector3>::iterator mit;
	int nb = 0;
	
	//rebuild vertex indices array
	getVertexIndiceArray();

	//take a temporary list for vertices
	std::list<Maths::Vector3> used_vertices;

	//loop on the vertex indices array
	for (unsigned int h = 0 ; h < vert_indices.size() ; h++) {
		//if the vertex i is indiced
		if (vert_indices[h].indices.size() > 0) {
			
			//add it to the list of used vertices
			used_vertices.push_back(vertices[h]);
			
			//update the indices array
			for (iit = vert_indices[h].indices.begin() ; iit != vert_indices[h].indices.end() ; iit++) {
				indices[(*iit)] = nb;
			}

			//increase the number of useful vertices
			nb++;

		}
	}

	//output for debug
	//std::cout << "Number of vertices removed: " << vertices.size() - nb << std::endl;

	//rebuild the vertiex array
	delete[] vertices;
	vertices = new Maths::Vector3[nb];
	int h = 0;
	for (mit = used_vertices.begin() ; mit != used_vertices.end() ; mit++) vertices[h++] = *mit;
	nb_vertices = nb;

	vert_indices_valid = false;

}

void MeshContainer::createSurfaceMeshFromVolumeMesh(MeshContainer& v_mesh) {

	//check mesh type
	if (nb_vert_per_elem != 3 || nb_face_per_elem != 3) {
		Message::error("Not possible to create surface mesh in a non surface mesh");
		return;
	}

	int* t1 = new int[v_mesh.getNBVertIndicesPerFace()];

	int nb_points = 0;
	int nb_ind = 0;

	//points mapping for O(n) algorithm
	int* point_map = new int[v_mesh.nb_vertices];
	for (int k = 0 ; k < v_mesh.nb_vertices ; k++) point_map[k] = -1;
	int* indice_tmp = new int[v_mesh.nb_elements*12];

	//loop over the elements to count the number of point and faces
	for (int e = 0 ; e < v_mesh.nb_elements ; e++) {

		//for all its neighbors
		for (int f = 0 ; f < v_mesh.getNbFacePerElem() ; f++) {
			
			//check if the face has no neighbor => outside face!
			if (v_mesh.elements[e].neighbors[f] == NULL) {

				//get the vertices indices for the face
				v_mesh.getElemFaceVerticesIndices(e, f, t1);

				//add it to point list and index map
				for (int k = 0 ; k < v_mesh.getNBVertIndicesPerFace() ; k++) {
					//add the point the the point list
					if (point_map[t1[k]] == -1) {
						point_map[t1[k]] = nb_points;
						nb_points++;
					}
					indice_tmp[nb_ind++] = point_map[t1[k]];
				}
			}
		}

	}

	//copy useful information
	nb_vertices = nb_points;
	nb_indices = nb_ind;

	//browse the indice map to create the points
	delete[] vertices;
	vertices = new Maths::Vector3[nb_points];

	for (int k = 0 ; k < v_mesh.nb_vertices ; k++) {
		if (point_map[k] != -1) {
			//copy the point
			vertices[point_map[k]] = v_mesh.vertices[k];
		}
	}

	//copy the indices
	delete[] indices;
	indices = new int[nb_ind];
	for (int k = 0 ; k < nb_ind ; k++) {
		indices[k] = indice_tmp[k];
	}

	//final treatments
	delete[] t1;
	delete[] point_map;
	delete[] indice_tmp;

	initElements();
	computeBoundedBox();
	computeElementsNeighbors();
	
}

void MeshContainer::computeBoundedBox() {

	if (nb_vertices < 1) return;

	Maths::Vector3 min = vertices[0];
	Maths::Vector3 max = vertices[0];

	for (int i = 1 ; i < nb_vertices ; i++) {
		
		if (vertices[i].x < min.x) min.x = vertices[i].x;
		if (vertices[i].y < min.y) min.y = vertices[i].y;
		if (vertices[i].z < min.z) min.z = vertices[i].z;

		if (vertices[i].x > max.x) max.x = vertices[i].x;
		if (vertices[i].y > max.y) max.y = vertices[i].y;
		if (vertices[i].z > max.z) max.z = vertices[i].z;

	}

	bounded_box.setMinimum(min);
	bounded_box.setMaximum(max);

}

void MeshContainer::computeBoundingBoxFromIndices() {

	if (nb_indices < 1) return;

	Maths::Vector3 min = vertices[indices[0]];
	Maths::Vector3 max = vertices[indices[0]];

	for (int i = 1 ; i < nb_indices ; i++) {
		
		Maths::Vector3& v = vertices[indices[i]];

		if (v.x < min.x) min.x = v.x;
		if (v.y < min.y) min.y = v.y;
		if (v.z < min.z) min.z = v.z;

		if (v.x > max.x) max.x = v.x;
		if (v.y > max.y) max.y = v.y;
		if (v.z > max.z) max.z = v.z;

	}

	bounded_box.setMinimum(min);
	bounded_box.setMaximum(max);

}

void MeshContainer::getFacesNormals(Maths::Vector3* normals, float angle_non_smooth) {

	//check mesh type
	if (nb_vert_per_elem != 3 || nb_face_per_elem != 3) {
		Message::error("Not possible to compute normal on a non surface mesh");
		return;
	}

	float cos_lim = cos(angle_non_smooth);

	int h = 0;
	Maths::Vector3 base_n, sum_n, n;
	int nb_n;
	Maths::Vector3 vert[3];
	int start;
	int e2;
	int ind;

	//for each face
	for (int e = 0 ; e < nb_elements ; e++) {

		start = e*nb_vert_per_elem;

		//get the 3 first vertices of the element face f
		for (int v = 0 ; v < nb_vert_per_elem ; v++) vert[v] = vertices[indices[start + v]];
		base_n = (vert[1] - vert[0]).crossProduct(vert[2] - vert[0]);
		base_n.normalise();

		//for each vertex of this face
		for (int v = 0 ; v < nb_vert_per_elem ; v++) {

			sum_n = base_n;
			nb_n = 1;

			ind = indices[e*nb_vert_per_elem + v];

			//for each face attached to this vertex
			for (unsigned int fa = 0 ; fa < vert_indices[ind].indices.size() ; fa++) {

				//get the normal of the face
				e2 = (int)(vert_indices[ind].indices[fa]/nb_vert_per_elem);
				if (e2 == e) continue;
				start = e2*nb_vert_per_elem;
				for (int v = 0 ; v < nb_vert_per_elem ; v++) vert[v] = vertices[indices[start + v]];
				n = (vert[1] - vert[0]).crossProduct(vert[2] - vert[0]);
				n.normalise();

				//if the normal of the face is close to the normal of 
				float cos_ang = float(base_n.dotProduct(n));
				if (cos_ang > cos_lim) {
					sum_n += n;
					nb_n++;
				}

			}

			//set the normal for this point
			normals[h] = sum_n/nb_n;
			normals[h].normalise();

			h++;

		}

	}

}

void MeshContainer::getElementCenter(int e_id, Maths::Vector3& center) {

	int start = e_id*nb_vert_per_elem;
	Maths::Vector3 sum = Maths::Vector3(0, 0, 0);
	for (int v = 0 ; v < nb_vert_per_elem ; v++) sum += vertices[indices[start + v]];

	center = sum*(1.0f/(float)nb_vert_per_elem);

}

void MeshContainer::transform(Maths::Matrix3& rotation, Maths::Vector3& translation) {

	for (int i = 0 ; i < nb_vertices ; i++) {
		vertices[i] = rotation*vertices[i] + translation;
	}

	//vertex dependant strucutre update
	computeBoundedBox();
	computeVertexGrid();

}

void MeshContainer::getElementVertices(int i, Maths::Vector3* pts) {
	
	int start = i*nb_vert_per_elem;
	int end = start + nb_vert_per_elem;
	int h = 0;
	for (int i = start ; i < end ; i++) {
		pts[h++] = vertices[indices[i]];
	}

}

void MeshContainer::getElemFaceVerticesIndices(int elem_id, int elem_face, int* out_indices) {

	int start = elem_id*nb_vert_per_elem;
	for (int h = 0 ; h < nb_indices_per_face ; h++) {
		out_indices[h] = indices[start + faces_vertices_indices[nb_indices_per_face*elem_face + h]];
	}

}

void MeshContainer::getElemFaceVerticesIndicesRelative(int elem_id, int elem_face, int* out_indices) {

	for (int h = 0 ; h < nb_indices_per_face ; h++) {
		out_indices[h] = faces_vertices_indices[nb_indices_per_face*elem_face + h];
	}

}

void MeshContainer::getElemFaceTriangleVerticesPos(int elem_id, int elem_face, int tri_id, Maths::Vector3* out_vert) {

	int start = elem_id*nb_vert_per_elem;
	for (int h = 0 ; h < nb_indices_per_face ; h++) {
		out_vert[h] = vertices[indices[start + faces_vertices_indices[nb_indices_per_face*elem_face + h]]];
	}

}


void MeshContainer::copyMeshPropertiesFrom(MeshContainer& src) {

	this->nb_vert_per_elem = src.nb_vert_per_elem;
	this->nb_face_per_elem = src.nb_face_per_elem;
	this->nb_vert_per_face = src.nb_vert_per_face;
	this->nb_indices_per_face = src.nb_indices_per_face;
	this->faces_vertices_indices = new int[this->nb_face_per_elem*this->nb_indices_per_face];
	memcpy(this->faces_vertices_indices, src.faces_vertices_indices, this->nb_face_per_elem*this->nb_indices_per_face*sizeof(int));

}

void MeshContainer::getElemFacesNormal(int elem_id, Maths::Vector3* out_normals) {

	int start = elem_id*nb_vert_per_elem;
	Maths::Vector3 vert[3];
	for (int f = 0 ; f < nb_face_per_elem ; f++) {
		//get the 3 first vertices of the element face f
		for (int v = 0 ; v < 3 ; v++) vert[v] = vertices[indices[start + faces_vertices_indices[nb_indices_per_face*f + v]]];
		//compute the corresponding normal vector
		out_normals[f] = (vert[1] - vert[0]).crossProduct(vert[2] - vert[0]);
		out_normals[f].normalise();
	}

}

bool MeshContainer::saveInBinaryFile(std::string file_name) {

	//create the file
    std::ofstream file;
    file.open(file_name.c_str(), std::ios_base::out | std::ios_base::binary);
	if (!file.good()) {
		Message::error("Can't create mesh file: " + file_name);
		return false;
	}

	//save data
	bool ret = saveInBinaryFile(file);

	//close file
    file.close();

	return ret;

}

bool MeshContainer::saveInBinaryFile(std::ofstream& file) {

	//vertices
	file.write((char*)(&nb_vertices), sizeof(int));
	//put vertices into a buffer to more efficient store
	int vert_size = sizeof(Maths::Real)*3*nb_vertices;
	Maths::Real* buffer = new Maths::Real[3*nb_vertices];
	int h = 0;
	for (int i = 0 ; i < nb_vertices ; i++) {
		buffer[h++] = vertices[i].x;
		buffer[h++] = vertices[i].y;
		buffer[h++] = vertices[i].z;
	}
	//write the buffer
	file.write((char*)(buffer), vert_size);	
	delete[] buffer;

	//indices
	file.write((char*)(&nb_indices), sizeof(int));
	file.write((char*)(indices), sizeof(int)*nb_indices);

	//super indices array
	int s, v;
	for (int i = 0 ; i < nb_vertices; i++) {
		VertexIndices& vn = vert_indices[i];
		s = vn.indices.size();
		file.write((char*)(&s), sizeof(int));
		for (unsigned int h = 0 ; h < vn.indices.size() ; h++) {
			v = vn.indices[h];
			file.write((char*)(&v), sizeof(int));
		}
	}

	//vertex grid
	/*file.write((char*)(&grid_nb_cells), sizeof(int));
	file.write((char*)(&gr), sizeof(int));
	for (int i = 0 ; i < grid_nb_cells; i++) {
		VertexIndices& vn = vertex_grid[i];
		s = vn.indices.size();
		file.write((char*)(&s), sizeof(int));
		for (int h = 0 ; h < vn.indices.size() ; h++) {
			v = vn.indices[h];
			file.write((char*)(&v), sizeof(int));
		}
	}*/

	return true;
	
}


bool MeshContainer::loadFromBinaryFile(std::string file_name) {

	//open the file
	std::ifstream file;
	file.open(file_name.c_str(), std::ios_base::in | std::ios_base::binary);
	if (!file.good()) {
		Message::debug("Can't open mesh file: " + file_name);
		return false;
	}

	//load data
	bool ret = loadFromBinaryFile(file);

	//close file
    file.close();

	return ret;

}

bool MeshContainer::loadFromBinaryFile(std::ifstream& file) {

	//vertices
	file.read((char*)(&nb_vertices), sizeof(int));
	delete[] vertices;
	vertices = new Maths::Vector3[nb_vertices];
	Maths::Real* buffer = new Maths::Real[3*nb_vertices];
	file.read((char*)(buffer), sizeof(Maths::Real)*3*nb_vertices);

	//copy data into the vertices array
	int h = 0;
	for (int i = 0 ; i < nb_vertices ; i++) {
		vertices[i].x = buffer[h++];
		vertices[i].y = buffer[h++];
		vertices[i].z = buffer[h++];
	}
	delete[] buffer;

	//indices
	file.read((char*)(&nb_indices), sizeof(int));
	indices = new int[nb_indices];
	file.read((char*)(indices), sizeof(int)*nb_indices);

	//super indices array
	vert_indices.resize(nb_vertices);
	int s, v;
	for (int i = 0 ; i < nb_vertices; i++) {
		VertexIndices& vn = vert_indices[i];
		file.read((char*)(&s), sizeof(int));
		vn.indices.resize(s);
		for (unsigned int h = 0 ; h < vn.indices.size() ; h++) {			
			file.read((char*)(&v), sizeof(int));
			vn.indices[h] = v;
		}
	}
	vert_indices_valid = true;

	return true;

}

void MeshContainer::free_memory() {

	delete[] indices;
	delete[] surface_indices;	
	delete[] faces_vertices_indices;
	delete[] elements_neighbors;

	if (!child_mesh) {
		delete[] vertices;
	}
	
	delete uniform_grid;
}

int MeshContainer::removeReference() {

	nb_references--;
	int nb = nb_references;

	if (nb_references <= 0) delete this;

	return nb;

}

void MeshContainer::copy(const MeshContainer& src) {

}

MeshContainer* MeshContainer::getSubMesh(int nb_elems, int* elem_indices, bool compute_meta_data) {
	
	MeshContainer* mc = new MeshContainer();

	int* vertex_map = new int[nb_vertices];			//map between the vertices of parent mesh and new mesh
	for (int i = 0 ; i < nb_vertices ; i++) vertex_map[i] = -1;
	int st_ind;
	int max_v = 0;

	mc->nb_indices = 0;
	mc->nb_elements = nb_elems;
	mc->indices = new int[nb_elems*nb_vert_per_elem];
	mc->nb_vertices = 0;
	//browse elements to create the vertex map and count the number of vertices
	//create the indices array at the same time
	for (int h = 0 ; h < nb_elems ; h++) {
		int e = elem_indices[h];
		st_ind = e*nb_vert_per_elem;
		for (int i = 0 ; i < nb_vert_per_elem ; i++) {
			int ind = indices[st_ind + i];
			if (ind > max_v ) max_v = ind;
			if (vertex_map[ind] == -1) vertex_map[ind] = mc->nb_vertices++;
			mc->indices[mc->nb_indices++] = vertex_map[ind];
		}
	}

	//allocate and create the vertices table
	mc->vertices = new Maths::Vector3[mc->nb_vertices];
	for (int h = 0 ; h < nb_vertices ; h++) {
		int v = vertex_map[h];
		if (v != -1) mc->vertices[v] = vertices[h];
	}

	delete[] vertex_map;

	//optionnal extra computations
	if (compute_meta_data) {
		mc->computeBoundedBox();
		mc->computeVertexGrid();
		mc->computeVerticesNeighbors();			//DEBUG
	}

	return mc;

}

float MeshContainer::getMeshElemVolume(float* elem_volume) {

	Maths::Vector3 elem_vert[4];

	float volume = 0.0f;

	Maths::Matrix3 x;					
	float det_x;

	//for each element
	for(int e = 0 ; e < nb_elements ; e++) {

		//get the four vertices of the elements
		getElementVertices(e, elem_vert);

		//compute auxiliary variables
		//basis of the element
		x.SetColumn(0, elem_vert[1] - elem_vert[0]);
		x.SetColumn(1, elem_vert[2] - elem_vert[0]);
		x.SetColumn(2, elem_vert[3] - elem_vert[0]);
		det_x = x.Determinant();
		elem_volume[e] = det_x/6.0f;
		volume = volume+ elem_volume[e]; 
	}

	return volume;

}

void MeshContainer::getCenterOfMass(Maths::Vector3& centerOfMass){
	
	// initialization
	float* elem_volume = new float[nb_elements];
	float volume;
	volume = getMeshElemVolume(elem_volume);
	Maths::Vector3 center;
	centerOfMass = Maths::Vector3::ZERO;


	// average of all elements in the mesh
	for(int i = 0; i < nb_elements; i++){
		getElementCenter(i, center);
		centerOfMass = centerOfMass + elem_volume[i]*center;
	}
	centerOfMass = (1.0f/volume)*centerOfMass;


	delete[] elem_volume;
}

void MeshContainer::getPrincipalAxes(Maths::Vector3* axes, Maths::Vector3 centerOfMass = Maths::Vector3::ZERO){

	//initialization 
	float a = 0.0f;
	float b = 0.0f;
	float c = 0.0f;
	float d = 0.0f;
	float e = 0.0f;
	float f = 0.0f;
	Maths::Vector3 center; 

	// calculus of the volume
	float* elem_volume = new float[nb_elements];
	float volume;
	volume = getMeshElemVolume(elem_volume);

	// Construction of the inertia matrix of the mesh
	for(int i = 0; i < nb_elements; i++){
		getElementCenter(i, center);
		a = a + elem_volume[i] * ((center.y - centerOfMass.y)* (center.y - centerOfMass.y) + (center.z - centerOfMass.z) * (center.z - centerOfMass.z));
		b = b + elem_volume[i] * ((center.x - centerOfMass.x) * (center.x - centerOfMass.x) + (center.z - centerOfMass.z) * (center.z - centerOfMass.z));
		c = c + elem_volume[i] * ((center.y - centerOfMass.y) * (center.y - centerOfMass.y) + (center.x - centerOfMass.x) * (center.x - centerOfMass.x));
		d = d - elem_volume[i] * ((center.x - centerOfMass.x) * (center.y - centerOfMass.y));
		e = e - elem_volume[i] * ((center.x - centerOfMass.x) * (center.z - centerOfMass.z));
		f = f - elem_volume[i] * ((center.y - centerOfMass.y) * (center.z - centerOfMass.z));
	}

	Maths::Matrix3 inertiaMatrix;
	inertiaMatrix = Maths::Matrix3(a, d, e, d, b, f, e, f, c);
	
	Maths::Real eigenValues[3];
	Maths::Vector3 eigenVectors[3];

	// the eigen vectors are the principal axes of the mesh
	inertiaMatrix.EigenSolveSymmetric(eigenValues, eigenVectors);

	//reorder axes
	int i_min1 = 0;
	int i_min2 = 0;
	for (int i = 1 ; i < 3 ; i++) {
		if (eigenValues[i] < eigenValues[i_min1]) i_min1 = i;
		else i_min2 = i;
	}
	for (int i = 0 ; i < 3 ; i++) {
		if (i != i_min1 && eigenValues[i] < eigenValues[i_min2]) i_min2 = i;
	}
	axes[0] = eigenVectors[i_min1];
	axes[1] = eigenVectors[i_min2];
	axes[2] = axes[0].crossProduct(axes[1]);

	delete[] elem_volume;

}

void MeshContainer::normalize() {

	// calculus of the volume
	float* elem_volume = new float[nb_elements];
	float volume;
	volume = getMeshElemVolume(elem_volume);

	// calculus of the center of mass
	Maths::Vector3 centerOfMass;
	getCenterOfMass(centerOfMass);	


	//(0, 0, 0) is the new center of mass of the mesh
	Maths::Matrix3 identity = Maths::Matrix3::IDENTITY;
	transform(identity, (-1.0f)*centerOfMass);


	// calculus of the principal axes
	Maths::Vector3 axes[3];
	getPrincipalAxes(axes);


	// gets the coordinates of principal axes, wich gives the inverse rotation to apply on the mesh
	Maths::Matrix3 base;
	base.FromAxes(axes[0], axes[1], axes[2]);
	Maths::Matrix3 invbase;
	invbase = base.Inverse();
	Maths::Vector3 zero = Maths::Vector3::ZERO;
	transform(invbase, zero);

	float vol = (float)pow((double) volume,(double) (1.0f/3.0f));

	//Normalize the volume, the new volume will be 1
	for(int i = 0; i < nb_vertices; i++){
		vertices[i] = (1.0f/vol) * vertices[i];
	}

	delete[] elem_volume;
}

void MeshContainer::clone(MeshContainer* new_mesh){

	// copy vertices
	new_mesh->nb_vertices = nb_vertices;
	Maths::Vector3* vert = new Maths::Vector3[new_mesh->nb_vertices];
	for(int i = 0; i < new_mesh->nb_vertices; i++) {
		vert[i] = vertices[i];
	}
	new_mesh->vertices = vert;

	// copy indices
	new_mesh->nb_indices = nb_indices;
	int* ind = new int[new_mesh->nb_indices];
	for(int i = 0; i < new_mesh->nb_indices; i++) {
		ind[i] = indices[i];
	}
	new_mesh->indices = ind;

	// copy surface_indices
	new_mesh->nb_surface_indices = nb_surface_indices;
	int* sur = new int[new_mesh->nb_surface_indices];
	for(int i = 0; i < new_mesh->nb_surface_indices; i++) {
		sur[i] = surface_indices[i];
	}
	new_mesh->surface_indices = sur;

	// copy child_mesh
	new_mesh->child_mesh = child_mesh;

	// copy elements
	new_mesh->nb_elements = nb_elements;
	new_mesh->elements = elements;

	// copy element_marker
	new_mesh->element_marker = element_marker;

	// copy bounded_box
	new_mesh->bounded_box = bounded_box;

	// copy vert_indices
	new_mesh->vert_indices = vert_indices;

	// copy nb_references
	new_mesh->nb_references = nb_references;

	new_mesh->initElements();
	new_mesh->computeElementsNeighbors();

}



MeshContainer& MeshContainer::operator=(const MeshContainer& src) {

	if (&src != this) {
		free_memory();
		copy(src);
	}

	return *this;

}