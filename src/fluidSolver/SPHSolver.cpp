#include "sphsolver.h"
#include "../geom/container.hpp"
#include "math.h"
#define _USE_MATH_DEFINES

SPHSolver::SPHSolver()
{
    k_stiffness = 1000;
    mu_viscosity = 25;
    h = .100001; //radius
    h9 = h*h*h*h*h*h*h*h*h;
    h2 = h*h;
    h6 = h*h*h*h*h*h;
    d_rest_density = 1000;
    m_mass = .125;
    dt_timestep = .001;
    usg = grid();
    epsilon = .003;
}


void SPHSolver::init(float r){
    //bound min/max of container = grid min and max
    //radius = how big each square is
    r = h;

    usg = grid(0, 2, h); // cell size should be related to h
}

void SPHSolver::update() {
    //clear grid
    for(int iter = 0; iter < usg.cells.size(); iter++) {
        //clear the list
        usg.cells.at(iter).clear();
    }
    
    // Step 1 - find neighbors for each particle and store them somewhere (usg)
    
    // Step x - collision detection & response (this should ensure that nothing is ever out of bounds for the next update())
    // compute cell indices of particle based on particle position
    int i,j,k;
    for(Particle* p: ParticlesContainer) {
        // before I find out index, check if particle is in bounds - collision detect

        
        // ASSUMPTION - we want to find index after ensuring that particle is in bounds
        i = floor(p->pos.x / usg.cell_size);
        j = floor(p->pos.y / usg.cell_size);
        k = floor(p->pos.z / usg.cell_size);
        
        if (p != nullptr){
            p->gridIndex = usg(i,j,k);
            usg.cells.at(p->gridIndex).push_back(p);
        }
        else{
            //whyare you here!?
            int x=1;
        }

    }
    
    for(int iter = 0; iter < usg.cells.size(); iter++) {
        //clear the list
        for(Particle* p: usg.cells.at(iter)) {
            if (p == nullptr) {
                int x =1;
            }
        };
    }

    
    //find density and pressure
    for(Particle* p: ParticlesContainer) {
        p->rho = accumulateDensity(p); //pass in cell to get density
        p->pres = calculatePressure(p->rho);
    }
    
    //resolve forces
    for(Particle* p: ParticlesContainer) {
        p->f_pressure = pressureForceDensity(p);
        p->f_gravity = p->rho * glm::vec3(0, -9.8, 0);
        p->force_density = p->f_pressure + p->f_gravity;
        
        //TODO: Implement f_viscosity to prevent blow up. Need to use "viscosity kernel"
    }

    //update speed
    for(Particle* p: ParticlesContainer) {
        p->speed += p->force_density*dt_timestep/p->rho;
        p->pos += p->speed*dt_timestep;
        
        //check if have gone out of bounds, collision detection
        // check particle's pos component, compare if it's lower than the bound component
        // if not in bounds (any of the 6 conditions trigger), then collision response
        // each will put the particle at respective bound.comp +- epsilon, same with velocity
        // p.pos.x < cont.bound.minX
        // <-. |
        //     |.-> p.vel.x *= vec3(-1,1,1)
        
        // p.pos.x > cont.bound.max
        if(p->pos.x > 2) { //hardcode for now, change later to access correct container scaleMax and mins
            p->pos.x = 2 - epsilon;
            p->speed *= glm::vec3(-0.1,1,1);
        } else if (p->pos.x < 0) {
            p->pos.x = 0 + epsilon;
        }
        
        if(p->pos.y > 2) { //hardcode for now, change later to access correct container scaleMax and mins
            p->pos.y = 2 - epsilon;
            p->speed *= glm::vec3(1,-0.1,1);
        } else if (p->pos.y < 0) {
            p->pos.y = 0 + epsilon;
        }
        
        if(p->pos.z > 2) { //hardcode for now, change later to access correct container scaleMax and mins
            p->pos.z = 2 - epsilon;
            p->speed *= glm::vec3(1,1,-0.1);
        } else if (p->pos.z < 0) {
            p->pos.z = 0 + epsilon;
        }

    }
    for(Particle* p: ParticlesContainer) {
        if (std::isnan(p->pos.x)){
            int a =1;
        }
    }

    
}

void SPHSolver::addParticle(Particle* p){

}

//neighbor search
std::vector<Particle*> SPHSolver::neighborSearchNaive(Particle* p){
    std::vector<Particle*> neighbors;
    for(Particle* n: ParticlesContainer) {
        if(glm::distance(p->pos, n->pos) < h) {
            p->a = 1;
            p->r = 0;
            p->g = 225;
            p->b = 0;
            neighbors.push_back(n);
        }
    }
    return neighbors;
}

//returns the grid cell (particle p + it's neighbors)
//circumvent this by having an epsilon value - takes more time to remove the particle
std::vector<Particle*> SPHSolver::neighborSearchUSG(Particle* p){
    for(Particle* n : usg.cells.at(p->gridIndex)) {
        p->a = 1;
        p->r = 0;
        p->g = 225;
        p->b = 0;
    }
    std::vector<Particle*> cell = usg.cells.at(p->gridIndex);
    return cell;
}

float SPHSolver::poly6_kernel(glm::vec3 pi_pos, glm::vec3 pj_pos){
    float x = glm::length2(pj_pos - pi_pos);
    if(epsilon < x && x < h + epsilon) {
        float constant = 315/(64 * M_PI * h9);
        float mult = (h2 - x*x)*(h2 - x*x)*(h2 - x*x);
        float ret_val = constant*mult; // clamp this to 0 min, because W is always non-negative
        if(ret_val < 0) return 0;
        return ret_val;
    } else {
        return 0;
    }
}

float SPHSolver::spiky_kernel(glm::vec3 pi_pos, glm::vec3 pj_pos){
    float x = glm::length2(pj_pos - pi_pos);
    if(epsilon < x && x < h + epsilon) {
        float constant = 15/(M_PI * h6);
        float mult = (h - x)*(h - x)*(h - x);
        return constant*mult;
    } else {
        return 0;
    }
}

float SPHSolver::accumulateDensity(const Particle* p){
    float rho = 0.f;
    // we could just call the func here
    if (usg.cells.at(p->gridIndex).size() > 0) {
        for(Particle* n : usg.cells.at(p->gridIndex)) {
            n->a = 1;
            n->r = 0;
            n->g = 225;
            n->b = 0;
            float kernel = poly6_kernel(p->pos, n->pos);
            rho += n->mass*kernel;
        }
        if (rho > 0) return rho;
    }
    return d_rest_density;
}


float SPHSolver::calculatePressure(float rho) {
    return fmax(0,k_stiffness*(rho - d_rest_density));
}

glm::vec3 SPHSolver::spiky_kernel_grad(glm::vec3 pi_pos, glm::vec3 pj_pos){
    float x = glm::length2(pj_pos - pi_pos);
    if(epsilon < x && x < h + epsilon) { // can never be sure of Float errors use +- EPSILON instead of h
        float constant = -45/(M_PI * h6);
        float mult = (h - x)*(h - x);
        return constant*mult*((pj_pos - pi_pos)/x);
    } else {
        return glm::vec3(0);
    }
}

glm::vec3 SPHSolver::pressureForceDensity(Particle* p){
    glm::vec3 PFD = glm::vec3(0);
    //summation equation
    for(Particle* n: usg.cells.at(p->gridIndex)) {
        glm::vec3 kernel = spiky_kernel_grad(p->pos, n->pos);
        float pressure = (p->pres + n->pres)/2;
        PFD += (n->mass / n->rho)*pressure*kernel;
    }
    return -(PFD);
}
