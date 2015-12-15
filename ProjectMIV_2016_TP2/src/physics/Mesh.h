
#ifndef MESH_H
#define MESH_H

#include "Particle.h"
#include <string>

class MeshContainer;

struct Mesh {

private:

	//for internal use
	MeshContainer* mc;

public:

	Mesh() : mc(NULL) {}
	~Mesh();

	//our mesh is defined by a set of particles (each of them have neighborhood information)
	std::vector<Particle> particles;

	//... and can be loaded from files using this function
	bool loadFromFile(std::string file_name);

	//accessor for internal use
	inline MeshContainer* getMeshContainer() { return mc; }

};

#endif