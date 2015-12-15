
#ifndef RIGID_SPHERE_H
#define RIGID_SPHERE_H

#include "AllMath.h"

class RigidSphere {

public:
	
	RigidSphere();

	//Draws the manipulator
	void Draw();
	void Move(float dx, float dy, float dz);

	inline void setPosition(Maths::Vector3 p) {pos = p;}
	inline void setRadius(Maths::Real r) {radius=r;}
	
	inline Maths::Real getRadius(){return radius;}
	inline Maths::Vector3 getPosition(){return pos;}

private:
	Maths::Vector3 pos;
	Maths::Real radius;
};

#endif