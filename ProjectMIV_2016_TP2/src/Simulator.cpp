#include "Simulator.h"
#include "Particle.h"
#include <iostream>
#include <string>
#include <fstream>

const float Simulator::GRAVITY_CONSTANT = 9.81f;
const float Simulator::K = 1000;//1000
const float Simulator::D = 40; //40
const float Simulator::dt = 1.0f/50.0f; // 100 à garder
const float Simulator::nb_iterations = 25; //25

void Simulator::Update()
{
	// Define the simulation loop (the methods are not in order)
	//ApplyVelocityDamping( ... )
	for (int j = 0; j < nb_iterations; j++) {

		ComputeForces();

		ApplyVelocityDamping();

		
		//fix first row of particules to simulate hanging
		for (int i = 0; i < 10; i++)
		{
			m_Mesh->particles[i].force_accumulator = Maths::Vector3(0, 0, 0);
		}
		/*
		// remove all forces on y axis starting y = 0 and below to simulate floor
		for (unsigned int p = 0; p < m_Mesh->particles.size(); p++)
		{
			if (m_Mesh->particles[p].pos.y <= 0)
			{
				m_Mesh->particles[p].force_accumulator.y = 0;
				m_Mesh->particles[p].vel.y = 0;
			}
		}
		*/

		//remove all forces and velocity to particles tagged as fixed
		for (unsigned int p = 0; p < m_Mesh->particles.size(); p++)
		{
			if (m_Mesh->particles[p].fixed)
			{
				m_Mesh->particles[p].force_accumulator = Maths::Vector3::ZERO;
				m_Mesh->particles[p].vel = Maths::Vector3::ZERO;
			}
		}

		Integrate();
		
		UpdateManipulator();
	}
} 

void Simulator::ComputeForces()
{
	for (unsigned int p = 0; p < m_Mesh->particles.size(); p++)
	{
		Particle *particle = &(m_Mesh->particles[p]);

		// gravity
		particle->force_accumulator = Maths::Vector3(0, -GRAVITY_CONSTANT, 0);
		
		// neighbors forces
		for (unsigned int n = 0; n < particle->neighbors.size(); n++)
		{
			Particle *neighbor = particle->neighbors[n];

			float dij = particle->pos.distance(neighbor->pos);
			float dij_init = particle->init_pos.distance(neighbor->init_pos);

			Maths::Vector3 diff_pos = neighbor->pos - particle->pos;
			diff_pos.normalise();

			particle->force_accumulator += diff_pos * K *  (dij - dij_init);
		}
	}
}

void Simulator::Integrate() 
{
	for (unsigned int p = 0; p < m_Mesh->particles.size(); p++)
	{
		if (!m_Mesh->particles[p].fixed)
		{
			Maths::Vector3 new_pos = m_Mesh->particles[p].pos + (m_Mesh->particles[p].vel * (dt / nb_iterations));
			Maths::Vector3 new_vel = m_Mesh->particles[p].vel + (m_Mesh->particles[p].force_accumulator * ((dt / nb_iterations) / m_Mesh->particles[p].mass));

			// update particle with new values
			m_Mesh->particles[p].pos = new_pos;
			m_Mesh->particles[p].vel = new_vel;
		}
	}
}

void Simulator::ApplyVelocityDamping()
{
	for (unsigned int p = 0; p < m_Mesh->particles.size(); p++)
	{
		Particle *particle = &(m_Mesh->particles[p]);

		for (unsigned int n = 0; n < particle->neighbors.size(); n++)
		{
			Particle *neighbor = particle->neighbors[n];

			Maths::Vector3 vrel = neighbor->vel - particle->vel;

			Maths::Vector3 diff_pos = particle->pos - neighbor->pos;
			diff_pos.normalise();

			particle->force_accumulator += diff_pos * (diff_pos.dotProduct(vrel) * D);
		}

		//add some friction
		particle->force_accumulator += -D * particle->vel;
	}
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

			if (m_Mesh->particles[p].pos.distance(manipulator_pos) < manipulator_radius && !m_Mesh->particles[p].fixed)
			{
				//m_Mesh->particles[p].fixed = true;
				
				m_force = m_Mesh->particles[p].pos - manipulator_pos;
				m_force.normalise();	
				m_Mesh->particles[p].pos = manipulator_pos + m_force * manipulator_radius;		

				//impose velocity ZERO perpendicular to the sphere surface
				m_Mesh->particles[p].vel -= m_force*m_Mesh->particles[p].vel.dotProduct(m_force);

			}
		}
	}
}

void Simulator::fixParticlesinSphere(RigidSphere* sphere)
{
	Maths::Vector3 sphere_pos = sphere->getPosition();
	Maths::Real sphere_radius = sphere->getRadius();

	for (unsigned int p = 0; p < m_Mesh->particles.size(); p++)
	{
		if (m_Mesh->particles[p].pos.distance(sphere_pos) < sphere_radius)
		{
			m_Mesh->particles[p].fixed = true;
		}
	}
}

void Simulator::checkCut() 
{
	Maths::Vector3 scalpel_pos = m_Scalpel->getPosition();
	Maths::Real scalpel_radius = m_Scalpel->getRadius();

	for (unsigned int p = 0; p < m_Mesh->particles.size(); p++)
	{
		Particle *particle = &(m_Mesh->particles[p]);

		for (unsigned int n = 0; n < particle->neighbors.size(); n++)
		{
			Particle *neighbor = particle->neighbors[n];

			
			// trouver le point median entre particle et neighbor
			Maths::Vector3 median_pos = particle->pos.midPoint(neighbor->pos);

			if (median_pos.distance(scalpel_pos) < scalpel_radius)
			{
			// retirer particle de neighbor.neighbors et neighbor de particle.neighbors
			CutLinks(particle, neighbor);
			}
			
			/*
			// calculer la distance entre le scalpel et le vecteur reliant les deux particules
			Maths::Vector3 link = Maths::Vector3(neighbor->pos.x - particle->pos.x, neighbor->pos.y - particle->pos.y, neighbor->pos.z - particle->pos.z);
			Maths::Vector3 particle_scalpel = Maths::Vector3(particle->pos.x - scalpel_pos.x, particle->pos.y - scalpel_pos.y, particle->pos.z - scalpel_pos.z);
			Maths::Vector3 neighbor_scalpel = Maths::Vector3(neighbor->pos.x - scalpel_pos.x, neighbor->pos.y - scalpel_pos.y, neighbor->pos.z - scalpel_pos.z);
			
			Maths::Real distance;
			distance = link.crossProduct(particle_scalpel).length / link.length;

			if (distance < scalpel_radius && particle_scalpel.dotProduct(scalpel_pos) < particle->pos.distance(neighbor->pos) && neighbor_scalpel.dotProduct(scalpel_pos) < particle->pos.distance(neighbor->pos))
			{
				// retirer particle de neighbor.neighbors et neighbor de particle.neighbors
				CutLinks(particle, neighbor);
			}*/
		}
	}
}

void Simulator::CutLinks(Particle* particle, Particle* neighbor)
{
	for (std::vector<Particle*>::iterator iter = particle->neighbors.begin(); iter != particle->neighbors.end(); ++iter)
	{
		if (*iter == neighbor)
		{
			particle->neighbors.erase(iter);
			break;
		}
	}

	for (std::vector<Particle*>::iterator iter = neighbor->neighbors.begin(); iter != neighbor->neighbors.end(); ++iter)
	{
		if (*iter == particle)
		{
			neighbor->neighbors.erase(iter);
			break;
		}
	}
}

void Simulator::checkOrphans()
{
	for (unsigned int p = 0; p < m_Mesh->particles.size(); p++)
	{
		if (m_Mesh->particles[p].neighbors.size() == 0)
		{
			for (std::vector<Particle>::iterator iter = m_Mesh->particles.begin(); iter != m_Mesh->particles.end(); ++iter)
			{
				if (&(*iter) == &(m_Mesh->particles[p]))
				{
					m_Mesh->particles.erase(iter);
					break;
				}
			}
		}
	}
}

void Simulator::fixParticles()
{
	if (m_Manipulator)
	{
		Maths::Vector3 manipulator_pos = m_Manipulator->getPosition();
		Maths::Real manipulator_radius = m_Manipulator->getRadius();

		for (unsigned int p = 0; p < m_Mesh->particles.size(); p++)
		{
			
			if (m_Mesh->particles[p].pos.distance(manipulator_pos) < manipulator_radius)
			{
				m_Mesh->particles[p].fixed = true;
			}
		}
	}
}

void Simulator::saveFixedParticles()
{
	std::ofstream f;
	f.open("save.txt");

	if (f.is_open()){
		for (unsigned int p = 0; p < m_Mesh->particles.size(); p++)
		{
			if (m_Mesh->particles[p].fixed)
			{
				f << p << "\n";
			}
		}
	}
	f.close();
}

void Simulator::dropMesh(float offset)
{
	for (unsigned int p = 0; p < m_Mesh->particles.size(); p++)
	{
		m_Mesh->particles[p].pos.y -= offset;
	}
}

void Simulator::restoreFixedParticles()
{
	std::ifstream f("save.txt");

	if (f)
	{
		std::string line;

		while (std::getline(f, line))
		{
			m_Mesh->particles[atoi(line.c_str())].fixed = true;
		}
	}
	f.close();
}

void Simulator::translateMesh(Maths::Vector3 t)
{
	for (unsigned int p = 0; p < m_Mesh->particles.size(); p++)
	{
		m_Mesh->particles[p].pos -= t;
	}
	
}

void Simulator::rotateMesh(Maths::Vector3 prevPos, Maths::Vector3 currPos)
{
	// get rotation quaternion
	Maths::Quaternion q;
	q = prevPos.getRotationTo(currPos);

	for (unsigned int p = 0; p < m_Mesh->particles.size(); p++)
	{
		m_Mesh->particles[p].pos = q * m_Mesh->particles[p].pos;
	}
}