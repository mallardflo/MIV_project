#include "Simulator.h"

const float Simulator::GRAVITY_CONSTANT = 9.81f;
const float Simulator::K = 500;
const float Simulator::D = 5;
const float Simulator::dt = 1.0f/100.0f;
const float Simulator::nb_iterations = 1;

void Simulator::Update()
{
	// Define the simulation loop (the methods are not in order)
	//ApplyVelocityDamping( ... )
	//ComputeForces( ... ) 
	//Integrate ( ... )
	//UpdateManipulator ( ... )
}

void Simulator::ComputeForces()
{
	
}

void Simulator::Integrate() 
{
	
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