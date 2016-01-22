
#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "Mesh.h"
#include "RigidSphere.h"
#include <ctime>

class Simulator {

	//Mesh updated during the simulation
	Mesh* m_Mesh;
	RigidSphere* m_Manipulator;
	RigidSphere* m_Scalpel;

	//Gravity constant
	static const float GRAVITY_CONSTANT;

	//Stifness constant
	static const float K;
	
	//damping constant of the springs
	static const float D;

	//number of iterations for each simulation step
	static const float nb_iterations;

	//simulation step
	static const float dt;

	//penalty variable
	static const float penalty;

public:

	//Constructor
	Simulator() {
	};

	int score;

	//Initialize the mesh
	inline void setMesh(Mesh* m) {m_Mesh = m;}

	//Initialize the manipulator
	inline void setManipulator(RigidSphere* m) {m_Manipulator = m;}

	//Initialize the scalpel
	inline void setScalpel(RigidSphere* s) {m_Scalpel = s;}

	//Simulation step
	void Update();

	Maths::Vector3 getForce() {return Maths::Vector3(1.5,1.5,1.5);}

	void fixParticlesinSphere(RigidSphere* s);

	void fixParticles();

	void saveFixedParticles();

	void restoreFixedParticles();
	
	void dropMesh(float offset);

	void translateMesh(Maths::Vector3 t);

	void rotateMesh(Maths::Vector3 prevPos, Maths::Vector3 currPos);

	void checkCut();

	void checkOrphans();

	void dontTouchFixedparticles();

private:

	//Updates the state of the mesh before display
	void ComputeForces();

	//Integrates the state of each particle of the mesh by computing new velocities and position at time t + dt
	void Integrate();
	
	void ApplyVelocityDamping();
	
	void UpdateManipulator();
	
	void CutLinks(Particle* particle, Particle* neighbor);
};

#endif