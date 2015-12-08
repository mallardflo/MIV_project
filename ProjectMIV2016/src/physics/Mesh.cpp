
#include "Mesh.h"
#include "MeshContainer.h"
#include "StringExtension.h"

bool Mesh::loadFromFile(std::string file_name) {

	//load the file using a mesh container
	std::string ext;

	if (!StringExtension::getFileExtension(ext, file_name)) return false;

	if (ext.compare("obj") == 0) {
		mc = new MeshContainer(MESH_TYPE_TRIANGLES);
	} else {
		mc = new MeshContainer();
	}

	//load the file
	bool ret = mc->loadFromFile(file_name);

	//copy the informations
	particles.resize(mc->nb_vertices);
	for (int v = 0 ; v < mc->nb_vertices ; v++) {
		particles[v].pos = particles[v].init_pos = mc->vertices[v];

		//neighbors
		particles[v].neighbors.resize(mc->vert_neighbors[v].neighbors.size());
		for (unsigned int n = 0 ; n < particles[v].neighbors.size() ; n++) {
			particles[v].neighbors[n] = &particles[mc->vert_neighbors[v].neighbors[n]];
		}
	}

	return ret;

}

Mesh::~Mesh() {
	delete mc;
}