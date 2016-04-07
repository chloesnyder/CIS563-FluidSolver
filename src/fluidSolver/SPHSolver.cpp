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
    h3 = h*h*h;
    d_rest_density = 1000;
    m_mass = 1;
    dt_timestep = .001;
    usg = grid();
    epsilon = .0001;
}

void SPHSolver::checkBounds(Particle* p) {
    if(p->pos.x > 2) { //hardcode for now, change later to access correct container scaleMax and mins
        p->pos.x = 2 - epsilon;
        p->speed *= glm::vec3(-0.05,1,1);
        //std::cout << "here  > x" << std::endl;
    } else if (p->pos.x < 0) {
        p->pos.x = 0 + epsilon;
        //std::cout << "here  < x" << std::endl;
    }
    
    if(p->pos.y > 2) { //hardcode for now, change later to access correct container scaleMax and mins
        p->pos.y = 2 - epsilon;
        p->speed *= glm::vec3(1,-0.05,1);
        //  std::cout << "here  < y" << std::endl;
    } else if (p->pos.y < 0) {
        p->pos.y = 0 + epsilon;
        //std::cout << "here  > y" << std::endl;
    }
    
    if(p->pos.z > 2) { //hardcode for now, change later to access correct container scaleMax and mins
        p->pos.z = 2 - epsilon;
        p->speed *= glm::vec3(1,1,-0.1);
        //std::cout << "here  < z" << std::endl;
    } else if (p->pos.z < 0) {
        p->pos.z = 0 + epsilon;
        // std::cout << "here  > z" << std::endl;
    }

}

void SPHSolver::init(){
    //bound min/max of container = grid min and max
    //radius = how big each square is

    usg = grid(0, 2, h/2); // cell size should be related to h
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
    //int i,j,k;
    
    //put particles in grid cells
    for(Particle* p: ParticlesContainer) {
        // before I find out index, check if particle is in bounds - collision detect
        checkBounds(p);
        
        // ASSUMPTION - we want to find index after ensuring that particle is in bounds
        p->i = floor((p->pos.x - usg.grid_min) / usg.cell_size);
        p->j = floor((p->pos.y - usg.grid_min) / usg.cell_size);
        p->k = floor((p->pos.z - usg.grid_min) / usg.cell_size);
        
        if (p != nullptr){
            p->gridIndex = usg(p->i,p->j,p->k);
            if(p->gridIndex > 0) {usg.cells.at(p->gridIndex).push_back(p);}
        }
        else{
            //whyare you here!?
            int x=1;
        }

    }
    
    //assign each particle its neighbors
    for(Particle* p: ParticlesContainer) {
        //clear what neighbors were there, repopulate
        p->neighbors.clear();
        p->neighbors = neighborSearchUSG(p);
    }
    
    //find density and pressure
    for(Particle* p: ParticlesContainer) {
        p->rho = accumulateDensity(p);
        p->pres = calculatePressure(p->rho);
    }
    
    //resolve forces
    for(Particle* p: ParticlesContainer) {
        p->f_pressure = pressureForceDensity(p);
        p->f_gravity = p->rho * glm::vec3(0, -9.8, 0);
        p->f_visc = viscForceDensity(p);
        p->force_density = p->f_pressure + p->f_gravity * p->f_visc;
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
        
        checkBounds(p);
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

std::vector<Particle*> SPHSolver::neighborSearchUSG(Particle* p){
    std::vector<Particle*> neighbors;
    for(int x = p->i - 1; x < p->i + 1; x++) {
        for(int y = p->j - 1; y < p->j + 1; y++) {
            for(int z = p->k - 1; z < p->k + 1; z++) {
                int idx = usg(x,y,z);
                if(idx > 0) {
                    neighbors.insert(neighbors.end(), usg.cells.at(idx).begin(), usg.cells.at(idx).end()); //insert the particles in the cell at the end of the neighbors vector
                }
            }
        }
    }
    return neighbors;
}

float SPHSolver::poly6_kernel(glm::vec3 pi_pos, glm::vec3 pj_pos){
    float x = glm::length(pi_pos - pj_pos);
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

glm::vec3 SPHSolver::spiky_kernel_grad(glm::vec3 pi_pos, glm::vec3 pj_pos){
    float x = glm::length(pi_pos - pj_pos);
    if(epsilon < x && x < h + epsilon) { // can never be sure of Float errors use +- EPSILON instead of h
        float constant = -45/(M_PI * h6);
        float mult = (h - x)*(h - x);
        glm::vec3 ret_val = constant*mult*((pi_pos - pj_pos)/x);
        if(ret_val.x < 0 && ret_val.y < 0 && ret_val.z < 0) return glm::vec3(0);
        return ret_val;
    } else {
        return glm::vec3(0);
    }
}


float SPHSolver::viscous_kernel_grad_2(glm::vec3 pi_pos, glm::vec3 pj_pos) {
    float x = glm::length2(pi_pos - pj_pos);
    if(epsilon < x && x < h + epsilon) {
        float constant = 45/(M_PI * h6);
        float mult = h - x;
        float visc_grad_2 = constant*mult; //does this need to be divided by x?
        if (visc_grad_2 > epsilon) {
            return visc_grad_2;
        }
    }
    return 0;
}

float SPHSolver::accumulateDensity(Particle* p){
    float rho = 0.f;
        for(Particle* n : p->neighbors){ //change this to be the neighbors now, not just the cell
            n->a = 1;
            n->r = 0;
            n->g = 225;
            n->b = 0;
            float kernel = poly6_kernel(p->pos, n->pos);
            rho += n->mass*kernel;
        }
//        std::cout << "RHO: " << rho << std::endl;
        if (fabs(rho) > epsilon){
//            std::cout << "Returning RHO as is" << std::endl;
            return rho;
        }
   // }
//    std::cout << "Returning RHO as REST DENSITY" << std::endl;
    return d_rest_density;
}


float SPHSolver::calculatePressure(float rho) {
    return fmax(0,k_stiffness*((rho + epsilon) - d_rest_density));
}

glm::vec3 SPHSolver::pressureForceDensity(Particle* p){
    glm::vec3 PFD = glm::vec3(0);
    //summation equation
    for(Particle* n: p->neighbors) {
        glm::vec3 kernel = spiky_kernel_grad(p->pos, n->pos);
        float pressure = (p->pres + n->pres)/2;
        if(pressure > epsilon)
        PFD += (n->mass / n->rho)*pressure*kernel;
    }
    return -(PFD);
}

glm::vec3 SPHSolver::viscForceDensity(Particle* p) {
    glm::vec3 VFD = glm::vec3(0);
    for(Particle* n: p->neighbors) {
        float kernel = viscous_kernel_grad_2(p->pos, n->pos);
        glm::vec3 uj_term = n->speed;
        glm::vec3 ui_term = p->speed;
        glm::vec3 visc = (uj_term - ui_term)*n->mass*kernel;
        if (p->rho > epsilon) {
            VFD += visc*mu_viscosity/p->rho;
            break;
        }
    }
    return VFD;
}
