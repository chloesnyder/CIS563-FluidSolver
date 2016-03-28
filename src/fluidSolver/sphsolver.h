#ifndef SPHSOLVER_H
#define SPHSOLVER_H

#include "grid.hpp"
#include "fluidSolver.hpp"

class SPHSolver : public fluidSolver
{
public:
    float k_stiffness;
    float mu_viscosity;
    float h; //radius
    float h9;
    float h2;
    float h6;
    float m_mass;

    float d_rest_density;
    float dt_timestep;
    grid usg;
    float epsilon;

    SPHSolver();
    void init(float r);
    void addParticle(Particle* p);

    //neighbor search
    std::vector<Particle*> neighborSearchNaive(Particle* p);
    std::vector<Particle*> neighborSearchUSG(Particle* p);

    float accumulateDensity(float mass, glm::vec3 pos, std::vector<Particle*> neighbors);
    float calculatePressure(float rho);
    glm::vec3 pressureForceDensity(Particle* p);
    void update(int t);


    //kernel functions
    float poly6_kernel(glm::vec3 pi_pos, glm::vec3 pj_pos); //density
    float spiky_kernel(glm::vec3 pi_pos, glm::vec3 pj_pos); //pressure, use gradient
    glm::vec3 spiky_kernel_grad(glm::vec3 pi_pos, glm::vec3 pj_pos);

};

#endif // SPHSOLVER_H
