
#ifndef PARTICLE_H
#define PARTICLE_H

#include "AllMath.h"

struct Particle {

	Maths::Vector3 init_pos;
	Maths::Vector3 prev_pos;		//position at the previuos frame (optionnal)
	Maths::Vector3 pos;
	Maths::Vector3 vel;

	bool fixed;

	Maths::Real mass;
	
	Maths::Vector3 force_accumulator;

	std::vector<Particle*> neighbors;

	//default values
	Particle() : pos(Maths::Vector3::ZERO), vel(Maths::Vector3::ZERO), mass(5.0f) , fixed(false){}

};

#endif
