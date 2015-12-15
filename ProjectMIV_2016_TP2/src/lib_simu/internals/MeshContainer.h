
#ifndef MESH_CONTAINER_H
#define MESH_CONTAINER_H

#include "AllMath.h"

#include "UniformGrid.h"

#include <vector>
#include <list>
#include <string>

#include <iostream>
#include <fstream>

#define NB_VERTEX_PER_CELL			8

enum MestType {
	MESH_TYPE_TETRAHEDRON = 0,
	MESH_TYPE_CUBES = 1,
	MESH_TYPE_DIAMONDS = 2,
	MESH_TYPE_TRIANGLES = 3,
};

/*! \struct MeshElement
 * \brief Data structure computed to facailitate mesh manipulation
 */
struct MeshElement {
	int id;
	int marker;
	MeshElement** neighbors;						/*!< NULL neighbor = no neighbor */
};

/*! \struct VertexNeighbors
 * \brief Data structure used temporary for neighboring computations
 */
struct VertexNeighbors {
	int marker;
	std::vector<int> neighbors;
};

/*! \struct VertexIndices
 * \brief Data structure used temporary for neighboring computations
 */
struct VertexIndices {
	std::vector<int> indices;
};
struct VertexIndicesList {
	std::list<int> indices;
};

/*! \class MeshContainer
 * \brief Container and loader for tetrahedra-based 3D volumetric meshes
 */
class MeshContainer {

private:

	void free_memory();
	void copy(const MeshContainer& src);

	int nb_references;							/*!< for automatic deallocation */

protected:

	/*!
     *  \brief Basic members initialisations
     */
	void initialize();

	/*!
     *  \brief Basic members initialisations
     */
	void initializeMeshType(MestType mesh_type);

	/*!
     * \brief Compute the bounded box of the mesh
	 */
	void computeBoundedBox();

	/*!
     * \brief Compute for each vertex the indices to the indices array that point to it. Very useful for fast neighboring computations.
	 */
	void getVertexIndiceArray();

	/*!
     * \brief Compute the neighboring of each element
	 */
	void computeElementsNeighbors();

	/*!
     * \brief Compute the neighboring of each vertex
	 */
	void computeVerticesNeighbors();

	/*!
     *  \brief Reorder the vertices trying to minimize the bandwidth of the adjacency graph of the vertices
     */
	void minimizeVerticesBandwidth();

	/*!
     *  \brief Reorder the elements trying to minimize the bandwidth of the adjacency graph of the element
     */
	void minimizeElementsBandwidth();

	/*!
     * \brief Remove all the vertices that are not indiced. This also correctly update the indices array.
	 */
	void removeUnusedVertices();

	//stats (times are in milliseconds)
	float time_loading;
	float time_super_index;
	float time_removing_unused_vertices;
	float time_computing_neighbors;
	float time_loading_total;

	UniformGrid* uniform_grid;						/*!< Uniform grid in which each cell store a list of vertex indice */

	//mesh element properties
	int nb_vert_per_elem;							/*!< number of vertices composing an element */
	int nb_face_per_elem;							/*!< number of face per element */
	int nb_vert_per_face;							/*!< number of vertice per element */
	int nb_indices_per_face;						/*!< number of vertice per element */
	int* faces_vertices_indices;					/*!< indices that describes how the faces of an element is formed */

	MeshElement** elements_neighbors;				/*!< We store all element neighboring information in a separate table for faster allocation and deallocation */

public:

	Maths::Vector3* vertices;						/*!< The list of vertices of the mesh */
	int* indices;									/*!< Indices of vertices. Each 4-group form a tetrahedron  */
	int nb_indices;
	int* surface_indices;							/*!< Indices of vertices. Each 3-group form a triangle  */
	int nb_surface_indices;
	bool child_mesh;								/*!< TRUE if the mesh uses other information from an other mesh object. Default to false. */

	int nb_elements;								/*!< Number of elements composing the mesh */
	int nb_vertices;								/*!< Number of vertices composing the mesh */

	std::vector<MeshElement> elements;				/*!< Array of tetrahedra. The order of the elements can be very important for mesh optimization */
	int element_marker;								/*!< integer used for fast element marking */

	bool vert_indices_valid;						/*!< true if vert_indices has valid entries */
	std::vector<VertexIndices> vert_indices;		/*!< Each ith entry is a list of indices to the indices array that point vertex i */
	std::vector<VertexNeighbors> vert_neighbors;	/*!< Each ith entry is a list of the vertices indices that are adjacent to the vertex i */
	int vertex_marker;								/*!< integer used for fast vertex marking */

	Maths::AxisAlignedBox bounded_box;				/*!< Bounded box of the loaded mesh */

	/*!
     *  \brief Load the mesh from a gmsh format
	 * \param file_name : full path to the file containing the ASCII gmsh formatted mesh information 
     */
	bool loadFromGmshFile(std::string file_name);

	/*!
     *  \brief Load the mesh from an off file associated to a "ele" file of the same nale
	 * \param file_name : full path to the file containing the ASCII off formatted mesh information 
     */
	bool loadFromOffFile(std::string file_name);
	bool loadFromObjFile(std::string file_name);

public:

	/*!
     *  \brief Constructor using pre-defined element composition
     */
	MeshContainer(MestType mesh_type = MESH_TYPE_TETRAHEDRON);
	/*!
     *  \brief Constructor for any type of element
	 * \param nb_vpe : number of vertices per element
	 * \param nb_fpe : number of vertices per element
	 * \param nb_fi : number of faces per element
	 * \param face_indices : indice array that indicate how is formed an element
     */
	MeshContainer(int nb_vpe, int nb_fpe, int nb_vpf, int nb_fi, int* face_indices);
	~MeshContainer();

	/*!
     * \brief Allocate elements and markers
	 */
	void initElements();

	/*!
     *  \brief Load the mesh from a file format
     */
	bool loadFromFile(std::string file_name, float scale = 1.0f, Maths::Vector3 offset = Maths::Vector3::ZERO, Maths::Matrix3 rotation = Maths::Matrix3::IDENTITY);

	/*!
     *  \brief From a volume mesh, creates a mesh whose element are triangle, representing the surface of the volume mesh
     */
	void createSurfaceMeshFromVolumeMesh(MeshContainer& v_mesh);

	/*!
     *  \brief Copy constructor (not implemented yet !!!)
     */
	MeshContainer(const MeshContainer& src);

	/*!
     *  \brief Copy the vertices of the ith element into the ouput buffer
	 * \param i : the indice of the element
	 * \param pts : out table to store the vertices coordinates
     */
	void getElementVertices(int i, Maths::Vector3* pts);

	/*!
     *  \brief Save in a binary file (*.mshdat) the content of the mesh array and meta data
     */
	bool saveInBinaryFile(std::string file_name);
	bool saveInBinaryFile(std::ofstream& file);

	/*!
     *  \brief Load from a binary file (*.mshdat) the content of the mesh array and meta data
     */
	bool loadFromBinaryFile(std::string file_name);
	bool loadFromBinaryFile(std::ifstream& file);

	/*!
	 * \brief apply a linear transformation to each point of the mesh
	 */
	void transform(Maths::Matrix3& rotation, Maths::Vector3& translation);

	/*!
     *  \brief copy operator =
     */
	MeshContainer& operator=(const MeshContainer& src);

	/*!
     *  \brief From a position, retrieve the id of the nearest vertex of the mesh
     */
	int getVertexFromPosition(Maths::Vector3& pos);

	/*!
     *  \brief Returns a new created mesh built from a subset of elements of this
	 * \param nb_elems: number of elements in the array elem_indices
	 * \param elem_indices: list of element indices that forms the sub mesh
	 * This method allocate a new mesh in heap memory. The caller of this function is responsible for deleting the retunred object.
     */
	MeshContainer* getSubMesh(int nb_elems, int* elem_indices, bool compute_meta_data = false);

	/*!
     *  \brief Returns the center position of element e_id
     */
	void getElementCenter(int e_id, Maths::Vector3& center);

	/*!
	 * \brief Initialize the elem_volume and the volume of the mesh
	 */
	float getMeshElemVolume(float* elem_volume);

	/*!
	 * \brief Calculates the center of mass of the mesh
	 */
	void getCenterOfMass(Maths::Vector3& centerOfMass);

	/*!
	 * \brief Calculates the main axes of the mesh, assuming that the center is (0, 0, 0)
	 */
	void getPrincipalAxes(Maths::Vector3* axes, Maths::Vector3 centerOfMass);

	/*!
	 * \brief Normalize the mesh, with a volume of 1 and with main axes matched with (X, Y, Z) axes
	 */
	void normalize();

	/*!
	 * \brief Returns a clone from the mesh
	 */
	void clone(MeshContainer* mesh);

	/*!
     * \brief Compute the bounded box of the mesh
	 */
	void computeBoundingBoxFromIndices();

	/*!
     * \brief Compute a uniform 3d grid in which each cell contain a list of vertices
	 */
	void computeVertexGrid();

	/*!
     * \brief Compute the normals of a surface mesh
	 */
	void getFacesNormals(Maths::Vector3* normals, float angle_non_smooth);

	/*!
     *  \brief Retrieve the indice list that describe an element face
     */
	void getElemFaceVerticesIndices(int elem_id, int elem_face, int* out_indices);
	void getElemFaceVerticesIndicesRelative(int elem_id, int elem_face, int* out_indices);
	void getElemFaceTriangleVerticesPos(int elem_id, int elem_face, int tri_id, Maths::Vector3* out_vert);
	void getElemFacesNormal(int elem_id, Maths::Vector3* out_normals);

	/*!
	 * copy all private properties of a mesh in another one
	 */
	void copyMeshPropertiesFrom(MeshContainer& src);

	/*!
	 * getters
	 */
	inline int getNbVertPerElem() { return nb_vert_per_elem; }
	inline int getNbFacePerElem() { return nb_face_per_elem; }
	inline int getNBVertPerFace() { return nb_vert_per_face; }
	inline int getNBVertTrianglePerFace() { return nb_indices_per_face/3; }
	inline int getNBVertIndicesPerFace() { return nb_indices_per_face; }

	/*!
	 * \brief increment the references counter for this mehsh
	 * \return the new number of references for this mesh
	 */
	inline int addReference() { nb_references++; return nb_references; }

	/*!
	 * \brief decrease the reference counter for this mesh
	 * If the reference counter is zero, the mesh is deleted
	 * \return the new number of references for this mesh
	 */
	int removeReference();

};


#endif