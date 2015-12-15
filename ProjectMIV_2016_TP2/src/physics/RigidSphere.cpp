
#include "RigidSphere.h"
#include "GUI.h"

RigidSphere::RigidSphere()
{
	radius = 1.0f;
	pos = Maths::Vector3::ZERO;
}

void RigidSphere::Move(float dx, float dy, float dz)
{
	pos.x += dx;
	pos.y += dy;
	pos.z += dz;
}

void RigidSphere::Draw()
{
	//Draw the manipulator
	GUI::beginTransformation(pos);
	GUI::setDrawColor(255, 0, 0);
	GUI::drawSphere(radius);
	GUI::endTransformation();
	
}