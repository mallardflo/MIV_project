#include "Simulator.h"
#include "Particle.h"

const float Simulator::GRAVITY_CONSTANT = 9.81f;
const float Simulator::K = 5000;
const float Simulator::D = 5;
const float Simulator::dt = 1.0f/800.0f;
const float Simulator::nb_iterations = 1;

void Simulator::Update(int n)
{
	// Define the simulation loop (the methods are not in order)
	//ApplyVelocityDamping( ... )
	for (int j = 0; j < n;j++) {
		ComputeForces();

		for (int i = 0; i < 10; i++)
		{
			m_Mesh->particles[i].force_accumulator = Maths::Vector3(0, 0, 0);
		}

		Integrate(n);
		//UpdateManipulator ( ... )
	}
} 

void Simulator::ComputeForces()
{
	for (unsigned int p = 0; p < m_Mesh->particles.size(); p++)
	{
		// gravity
		m_Mesh->particles[p].force_accumulator = Maths::Vector3(0, -GRAVITY_CONSTANT, 0);
		
		// neighbors forces
		Particle *particle = &(m_Mesh->particles[p]);

		for (unsigned int n = 0; n < particle->neighbors.size(); n++)
		{
			Particle *neighbor = particle->neighbors[n];

			float dij = particle->pos.distance(neighbor->pos);
			float dij_init = particle->init_pos.distance(neighbor->init_pos);

			Maths::Vector3 diff_pos = (neighbor->pos - particle->pos);

			diff_pos.normalise();

			particle->force_accumulator += diff_pos * K *  (dij - dij_init);
		}
	}
}

void Simulator::Integrate(int div_n) 
{
	for (unsigned int p = 0; p < m_Mesh->particles.size(); p++)
	{
		Maths::Vector3 new_pos = m_Mesh->particles[p].pos + (m_Mesh->particles[p].vel * (dt/div_n));
		Maths::Vector3 new_vel = m_Mesh->particles[p].vel + (m_Mesh->particles[p].force_accumulator * ((dt/div_n) / m_Mesh->particles[p].mass));
		
		// update particle with new values
		m_Mesh->particles[p].pos = new_pos;
		m_Mesh->particles[p].vel = new_vel;
	}
}

void Simulator::ApplyVelocityDamping()
{
	
}

void Simulator::UpdateManipulator()
{
	if(m_Manipulator)
	{
		Maths::Vector3 manipulator_pos = m_Manipulator->getPosition();
		Maths::Real manipulator_radius = m_Manipulator->getRadius();

		for (unsigned int p = 0 ; p < m_Mesh->particles.size() ; p++) 
		{
			//repulsion of the sphere
			Maths::Vector3 m_force = Maths::Vector3(0.0f, 0.0f, 0.0f);
			

			if (m_Mesh->particles[p].pos.distance(manipulator_pos) < manipulator_radius) 
			{
				m_force = m_Mesh->particles[p].pos - manipulator_pos;
				m_force.normalise();	
				m_Mesh->particles[p].pos = manipulator_pos + m_force * manipulator_radius;		

				//impose velocity ZERO perpendicular to the sphere surface
				m_Mesh->particles[p].vel -= m_force*m_Mesh->particles[p].vel.dotProduct(m_force);		
			}
		}
	}
}

void Simulator::CutLinks() 
{
	
}