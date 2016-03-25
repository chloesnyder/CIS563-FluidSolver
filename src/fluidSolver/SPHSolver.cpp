#include "sphsolver.h"
#include "../geom/container.hpp"

SPHSolver::SPHSolver()
{
    k_stiffness = 300;
    mu_viscosity = pow(10, -6);
    h = .225; //radius
    h9 = pow(h, 9);
    h2 = h*h;
    h6 = pow(h, 6);
    d_rest_density = 1000;
    m_mass = pow((2/3*h), 3) * d_rest_density;
    dt_timestep = 1;
    usg = grid();
    epsilon = .003;
}


void SPHSolver::init(float r){
    //bound min/max of container = grid min and max
    //radius = how big each square is

   usg = grid(glm::ivec3(ceil(2*boundX/r),ceil(2*boundY/r),ceil(2*boundZ/r)));

    //compute cell indices of particle based on particle position
    for(Particle* p : ParticlesContainer) {
        int i = floor(p->pos.x);
        int j = floor(p->pos.y);
        int k = floor(p->pos.z);
        usg.particles.at(usg(i,j,k)).push_back(p);
    }

    for(Particle* p1 : ParticlesContainer) {
        p1->rho = accumulateDensity(p1->mass, p1->pos, neighborSearchUSG(p1)); //pass in cell to get density
        p1->pres = calculatePressure(p1->rho);
    }
}

void SPHSolver::addParticle(Particle* p){

}

//neighbor search
std::vector<Particle*> SPHSolver::neighborSearchNaive(Particle* p){

}

//returns the grid cell (particle p + it's neighbors)
//circumvent this by having an epsilon value - takes more time to remove the particle
std::vector<Particle*> SPHSolver::neighborSearchUSG(Particle* p){
    int index = usg(p->pos.x, p->pos.y, p->pos.z);
    return usg.particles.at(index);
}

float SPHSolver::poly6_kernel(glm::vec3 pi_pos, glm::vec3 pj_pos){
    float x = glm::length2(pj_pos - pi_pos);
    if(epsilon <= x && x <= h) {
        float constant = 315/(64 * M_PI * h9);
        float mult = (h2 - x*x)*(h2 - x*x)*(h2 - x*x);
        return constant*mult;
    } else {
        return 0;
    }
}

float SPHSolver::spiky_kernel(glm::vec3 pi_pos, glm::vec3 pj_pos){
    float x = glm::length2(pj_pos - pi_pos);
    if(epsilon <= x && x <= h) {
        float constant = 15/(M_PI * h6);
        float mult = (h - x)*(h - x)*(h - x);
        return constant*mult;
    } else {
        return 0;
    }
}

float SPHSolver::accumulateDensity(float mass, glm::vec3 pos, std::vector<Particle*> neighbors) {
    float rho;

    for(Particle* n : neighbors) {
        float kernel = spiky_kernel(pos, n->pos);
        rho += n->mass*kernel;
    }
    return rho;
}

float SPHSolver::calculatePressure(float rho) {
    return k_stiffness*(rho - d_rest_density);
}
