#ifndef SPHSOLVER_H
#define SPHSOLVER_H

#include "grid.hpp"
#include "fluidSolver.hpp"


using namespace tbb;

class SPHSolver : public fluidSolver
{
public:
    float k_stiffness;
    float mu_viscosity;
    float h; //radius
    float h9;
    float h2;
    float h6;
    float h3;
    float m_mass;

    float d_rest_density;
    float dt_timestep;
    grid usg;
    float epsilon;
    float cellsize;
    SPHSolver();
    void init();
    void initParticles();
    
    /* ----------------
    NON TBB
     -----------*/
    
    //neighbor search
    std::vector<Particle*> neighborSearchNaive(Particle* p);
    concurrent_vector<Particle*> neighborSearchUSG(Particle* p);
    void not_TBB_update();
    
    float accumulateDensity(Particle* p);
    float calculatePressure(float rho);
    glm::vec3 pressureForceDensity(Particle* p);
    glm::vec3 viscForceDensity(Particle* p);
    void update();
    
    void checkBounds(Particle* p);


    //kernel functions
    float poly6_kernel(glm::vec3 pi_pos, glm::vec3 pj_pos); //density
    glm::vec3 spiky_kernel_grad(glm::vec3 pi_pos, glm::vec3 pj_pos); //pressure
    float viscous_kernel_grad_2(glm::vec3 pi_pos, glm::vec3 pj_pos); //visc
    
    /* ----------------
     TBB
     -----------*/
    
    //neighbor search
    concurrent_vector<Particle*> tbb_neighborSearchNaive(Particle* p);    
    float tbb_accumulateDensity(Particle* p);
    float tbb_calculatePressure(float rho);
    glm::vec3 tbb_pressureForceDensity(Particle* p);
    glm::vec3 tbb_viscForceDensity(Particle* p);
    
    
    //kernel functions
//    float poly6_kernel(glm::vec3 pi_pos, glm::vec3 pj_pos); //density
//    glm::vec3 spiky_kernel_grad(glm::vec3 pi_pos, glm::vec3 pj_pos); //pressure
//    float viscous_kernel_grad_2(glm::vec3 pi_pos, glm::vec3 pj_pos); //visc

};

#endif // SPHSOLVER_H
