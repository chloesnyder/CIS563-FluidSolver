#include "sphsolver.h"
#include "../geom/container.hpp"
#include "math.h"
#define _USE_MATH_DEFINES



SPHSolver::SPHSolver()
{
    k_stiffness = 1000;
    mu_viscosity = 25;
    h = .11001; //radius
    cellsize = 0.5f*h;

    h9 = h*h*h*h*h*h*h*h*h;
    h2 = h*h;
    h6 = h*h*h*h*h*h;
    h3 = h*h*h;
    
    d_rest_density = 1000;
    m_mass = 1;
    dt_timestep = .001;
    epsilon = .0001;
}

void SPHSolver::initParticles() {
    
    int particleId = 0;
    // hypothesis - initial particle separation should be less than h but
    // not so less that pressure forces completely explodes
    // what's the middle ground - it would be nice if someone wrote a paper about this
    particle_separation = 0.5*cellsize;
    Particle* p1 = new Particle();
    p1->pos = glm::vec3(.5, .5, .5);
    p1->id = 1;
    Particle* p2 = new Particle();
    p2 -> pos = glm::vec3(.5, .55, .5);
    p2->id = 2;
    //ParticlesContainer.push_back(p1);
    //ParticlesContainer.push_back(p2);
    
    for(float i = 0; i < .3; i += particle_separation) {
        for(float j = 0; j < 2; j += particle_separation) {
            for(float k = 0; k < 2; k += particle_separation) {
                Particle* P = new Particle();
                P->pos = glm::vec3(i,j,k);
                P->r = 0;
                P->g = 0;
                P->b = 200;
                P->a = 250;
                P->id = particleId++;
                ParticlesContainer.push_back(P);
            }
        }
    }
    
    MaxParticles = ParticlesContainer.size();
}



void SPHSolver::checkBounds(Particle* p) {
    float boundx = 4.f;
    float boundy = 3.f;
    float boundz = 2.f;
    if(p->pos.x > boundx) { //hardcode for now, change later to access correct container scaleMax and mins
        p->pos.x = boundx - epsilon;
        p->speed *= glm::vec3(-1,1,1);
        //std::cout << "here  > x" << std::endl;
    } else if (p->pos.x < 0) {
        p->pos.x = 0 + epsilon;
        p->speed *= glm::vec3(-1,1,1);
        //std::cout << "here  < x" << std::endl;
    }
    
    if(p->pos.y > boundy) { //hardcode for now, change later to access correct container scaleMax and mins
        p->pos.y = boundy - epsilon;
        p->speed *= glm::vec3(1,-1,1);
      //  std::cout << "here  < y" << std::endl;
    } else if (p->pos.y < 0) {
        p->pos.y = 0 + epsilon;
        p->speed *= glm::vec3(1, -.5, 1);
        //std::cout << "here  > y" << std::endl;
    }
    
    if(p->pos.z > boundz) { //hardcode for now, change later to access correct container scaleMax and mins
        p->pos.z = boundz - epsilon;
        p->speed *= glm::vec3(1,1,-1);
        //std::cout << "here  < z" << std::endl;
    } else if (p->pos.z < 0) {
        p->pos.z = 0 + epsilon;
        p->speed *= glm::vec3(1,1,-1);
        // std::cout << "here  > z" << std::endl;
    }

}

void SPHSolver::init(){
    usg = grid(0, 3, cellsize);
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
    
#define USE_TBB
#ifdef USE_TBB
    
    int particle_container_size = ParticlesContainer.size();
    
    parallel_for(0, particle_container_size, [&](int i) {
        //put particles in grid cells
        Particle* p = ParticlesContainer.at(i);
            // before I find out index, check if particle is in bounds - collision detect
            checkBounds(p);
            
            // ASSUMPTION - we want to find index after ensuring that particle is in bounds
            p->i = floor((p->pos.x - usg.grid_min) / usg.cell_size);
            p->j = floor((p->pos.y - usg.grid_min) / usg.cell_size);
            p->k = floor((p->pos.z - usg.grid_min) / usg.cell_size);
            
            if (p != nullptr && p != NULL){
                p->gridIndex = usg(p->i,p->j,p->k);
                if(p->gridIndex > 0 && p->gridIndex < usg.cells.size()) {usg.cells.at(p->gridIndex).push_back(p);}
            }
            else{
                int x=1;
            }
        
    });
   
    
    //assign each particle its neighbors
    parallel_for(0, particle_container_size, [&](int i) {
        Particle* p = ParticlesContainer.at(i);
        p->neighbors.clear();
        p->neighbors = neighborSearchUSG(p);
    });
  
    parallel_for(0,particle_container_size,[&](int i) {
        Particle* p = ParticlesContainer.at(i);
        p->rho = accumulateDensity(p);
        p->pres = calculatePressure(p->rho);
        //std::cout<<"density "<< p->rho<<std::endl;
        //I want density to be +/- 100 from rest density
    });
    
    //resolve forces
    parallel_for(0,particle_container_size,[&](int i) {
        Particle* p = ParticlesContainer.at(i);
        p->f_pressure = pressureForceDensity(p);
        p->f_gravity = p->rho * glm::vec3(0, -9.81, 0);
        p->f_visc = viscForceDensity(p);
        
        glm::vec3 press_sum = p->f_pressure + p->f_visc;
//        if(press_sum.y > (-1.f * p->f_gravity.y)) {
//            press_sum.y = 0;
//        }
        p->force_density = press_sum + p->f_gravity;
    });
    
    //update speed
    parallel_for(0,particle_container_size,[&](int i) {
        Particle* p = ParticlesContainer.at(i);
        //LEAPFROG METHOD http://www2.mpia-hd.mpg.de/~dullemon/lectures/fluiddynamics08/chap_10_sph.pdf
        p->pos = p->pos + .5f * dt_timestep * p->speed;
        p->speed = p->speed + (dt_timestep * p->force_density/p->rho);
        p->pos = p->pos + .5f * dt_timestep * p->speed;
        
        checkBounds(p);
    });
    
#else
    not_TBB_update();
#endif
}

void SPHSolver::not_TBB_update() {
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
            if(p->gridIndex > 0 && p->gridIndex < usg.cells.size()) {usg.cells.at(p->gridIndex).push_back(p);}
        }
        else{
            //whyare you here!?
            int x=1;
        }
        
    }
    
    //assign each particle its neighbors
    int a;
    for (int i=0; i<ParticlesContainer.size(); ++i){
        //clear what neighbors were there, repopulate
        Particle* p = ParticlesContainer.at(i);
        p->neighbors.clear();
        p->neighbors = neighborSearchUSG(p);
    }
    
       for(Particle* p: ParticlesContainer) {
        p->rho = accumulateDensity(p);
        p->pres = calculatePressure(p->rho);
        //std::cout<<"density "<< p->rho<<std::endl;
        //I want density to be +/- 100 from rest density
    }
    
    //resolve forces
    for(Particle* p: ParticlesContainer) {
        p->f_pressure = pressureForceDensity(p);
        p->f_gravity = p->rho * glm::vec3(0, -9.81, 0);
        p->f_visc = viscForceDensity(p);
        
        glm::vec3 press_sum = p->f_pressure + p->f_visc;
//        if(press_sum.y > (-1.f * p->f_gravity.y)) {
//            press_sum.y = 0;
//        }
        p->force_density = press_sum + p->f_gravity;
    }
    
    //update speed
    for(Particle* p: ParticlesContainer) {
        
        //LEAPFROG METHOD http://www2.mpia-hd.mpg.de/~dullemon/lectures/fluiddynamics08/chap_10_sph.pdf
        p->pos = p->pos + .5f * dt_timestep * p->speed;
        p->speed = p->speed + (dt_timestep * p->force_density/p->rho);
        p->pos = p->pos + .5f * dt_timestep * p->speed;
        
        checkBounds(p);
    }
}

//neighbor search
std::vector<Particle*> SPHSolver::neighborSearchNaive(Particle* p){
    std::vector<Particle*> neighbors;
    for(Particle* n: ParticlesContainer) {
        if(glm::distance(p->pos, n->pos) < h) {
            neighbors.push_back(n);
        }
    }
    return neighbors;
}

concurrent_vector<Particle*> SPHSolver::neighborSearchUSG(Particle* p){
    concurrent_vector<Particle*> neighbors;
    for(int x = p->i - 1; x <= p->i + 1; x++) {
        for(int y = p->j - 1; y <= p->j + 1; y++) {
            for(int z = p->k - 1; z <= p->k + 1; z++) {
                int idx = usg(x,y,z);

                if(idx > 0 && idx < usg.cells.size() - 1 ) {
                    // DEBUGGED / LEARNING - check if none of my neighbors have any particles - THIS IS BAD for SPH!
                    if (neighbors.size() > 40) {
                        break;
                    }
                    for(Particle* pp : usg.cells.at(idx)) {
                        neighbors.push_back(pp);
                    }
                   // neighbors.insert(neighbors.end(), usg.cells.at(idx).begin(), usg.cells.at(idx).end()); //insert the particles in the cell at the end of the neighbors vector
                }
            }
        }
    }

    return neighbors;
}

float SPHSolver::poly6_kernel(glm::vec3 pi_pos, glm::vec3 pj_pos){
    float x = glm::length(pi_pos - pj_pos);
    if(epsilon < x && x < h) {
        float constant = 315.f/(64.f * M_PI * h9);
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
    if(epsilon < x && x < h) { // can never be sure of Float errors use +- EPSILON instead of h
        float constant = -45.f/(M_PI * h6);
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
    if(epsilon < x && x < h) {
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
            if(p->id == n->id) continue;
            
            //LEARNING: pressure is high when density is either really high or really low, deviating from the rest density
            //SOLUTION IDEA: stop accumulating density at this point?
            if(rho >= d_rest_density + 100) {
                break;
            }
            float kernel = poly6_kernel(p->pos, n->pos);
            rho += n->mass*kernel;
            
        }
    
    if(d_rest_density - 100 < rho && rho <= d_rest_density + 100) {
        return rho;
    } else if (d_rest_density - 100 < d_rest_density + rho && d_rest_density + rho <= d_rest_density + 100) {
        return d_rest_density + rho;
    }
    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    return d_rest_density + r*100;
}


float SPHSolver::calculatePressure(float rho) {
    float pressure = (k_stiffness*(rho - d_rest_density));
    return pressure;
}

glm::vec3 SPHSolver::pressureForceDensity(Particle* p){
    glm::vec3 PFD = glm::vec3(0.f);
    //summation equation
    for(Particle* n: p->neighbors) {
        if(p->id == n->id) continue;
        glm::vec3 kernel = spiky_kernel_grad(p->pos, n->pos);
        float pressure = (p->pres + n->pres)/2;
//        if(pressure > epsilon)
        PFD += -(n->mass / n->rho)*pressure*kernel;
    }
    return PFD;
}

glm::vec3 SPHSolver::viscForceDensity(Particle* p) {
    glm::vec3 VFD = glm::vec3(0);
    for(Particle* n: p->neighbors) {
        if(p->id == n->id) continue;
        float kernel = viscous_kernel_grad_2(p->pos, n->pos);
        glm::vec3 uj_term = n->speed;
        glm::vec3 ui_term = p->speed;
        glm::vec3 visc = (uj_term - ui_term)*n->mass*kernel;
        if (p->rho > epsilon) {
            VFD += visc*mu_viscosity/p->rho;
           // break;
        }
    }
    return VFD;
}
