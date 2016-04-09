#include "sphsolver.h"
#include "../geom/container.hpp"
#include "math.h"
#define _USE_MATH_DEFINES



SPHSolver::SPHSolver()
{
    k_stiffness = 1000;
    mu_viscosity = 25;
    h = .10001; //radius
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
    
    for(float i = 0.5; i < boundX; i += particle_separation) {
        for(float j = 0.5; j < boundY; j += particle_separation) {
            for(float k = 0.5; k < boundZ; k += particle_separation) {
                Particle* P = new Particle();
                P->pos = glm::vec3(i,j,k);
                P->r = 225;
                P->g = 0;
                P->b = 0;
                P->a = 250;
                P->id = particleId++;
                ParticlesContainer.push_back(P);
            }
        }
    }
    
    //    Particle* pp = new Particle();
    //    pp->pos = glm::vec3(-2, -2, -2);
    //    pp->r = 0;
    //    pp->g = 0;
    //    pp->b = 225;
    //    pp->a = 250;
    //    ParticlesContainer.push_back(pp);
    
    MaxParticles = ParticlesContainer.size();
}



void SPHSolver::checkBounds(Particle* p) {
    float bound = 2.f;
    if(p->pos.x > bound) { //hardcode for now, change later to access correct container scaleMax and mins
        p->pos.x = bound - epsilon;
        p->speed *= glm::vec3(-0.001,1,1);
        //std::cout << "here  > x" << std::endl;
    } else if (p->pos.x < 0) {
        p->pos.x = 0 + epsilon;
        p->speed *= glm::vec3(-0.001,1,1);
        //std::cout << "here  < x" << std::endl;
    }
    
    if(p->pos.y > bound) { //hardcode for now, change later to access correct container scaleMax and mins
        p->pos.y = bound - epsilon;
        p->speed *= glm::vec3(1,-0.001,1);
      //  std::cout << "here  < y" << std::endl;
    } else if (p->pos.y < 0) {
        p->pos.y = 0 + epsilon;
        p->speed *= glm::vec3(1, -0.001, 1);
        //std::cout << "here  > y" << std::endl;
    }
    
    if(p->pos.z > bound) { //hardcode for now, change later to access correct container scaleMax and mins
        p->pos.z = bound - epsilon;
        p->speed *= glm::vec3(1,1,-0.001);
        //std::cout << "here  < z" << std::endl;
    } else if (p->pos.z < 0) {
        p->pos.z = 0 + epsilon;
        p->speed *= glm::vec3(1,1,-0.001);
        // std::cout << "here  > z" << std::endl;
    }

}

void SPHSolver::init(){
    //bound min/max of container = grid min and max
    //radius = how big each square is

    usg = grid(0, 2, cellsize); // cell size should be related to h
    // yes bigger cell size gives me more neighbors per cell
    // but eventually my neighbors run out
    // WHY?
    
    // hypothesis - it looks like too many particles are in a small space - > causing pressure force to separate them,
    // but the scale of that force is too high.
    
    // What could fix this?
    // - stiffness too high => pressure response is very high
    // - why are particles so close? => particle separation
        // what should I ideally set particle separation to?
            // What should it depend on?
                // multiple of h -> why?
                // i want to seed particles at less than the radius in which I search for neighbors
                // true - > but what actually is the radius in which i am search
    // WHITEBOARDING / LEARNING
                    // we realize that neighbor search radius in the grid cell is connected to finding enough neighbors
                    // so, to reach a cell ± 1, we do h = (3/2)*cellsize or cellsize = (2/3)*h
    
    // this seems to give me no neighbors - why?
    // is my particle separation too large - am I seeding particles initially so that they will be in radius h
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
//    for(Particle* p: ParticlesContainer) {
        //clear what neighbors were there, repopulate
        Particle* p = ParticlesContainer.at(i);
        p->neighbors.clear();
        p->neighbors = neighborSearchUSG(p);
        
        a = p->neighbors.size();
        int x=1;
        int y=2;
    }
    
//    for(Particle* p: ParticlesContainer) {
//        //clear what neighbors were there, repopulate
//        if (p->neighbors.size() < 10) {
//            std::cout<<"WHYYYYY I HAVE NO FRIENDS";
//        }
//    }

//    //find density and pressure
//    for(Particle* p: ParticlesContainer) {
//    //how many neighbors do I start with
//        //if (p->neighbors.size() <30)
//           // std::cout<<"#neighbors "<<p->neighbors.size()<<std::endl;
//    }
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
        p->force_density = p->f_pressure + p->f_gravity + p->f_visc;
    }

    //update speed
    for(Particle* p: ParticlesContainer) {
//        p->speed += p->force_density*dt_timestep/p->rho;
//        p->pos += p->speed*dt_timestep;
        
        //LEAPFROG METHOD http://www2.mpia-hd.mpg.de/~dullemon/lectures/fluiddynamics08/chap_10_sph.pdf
        p->pos = p->pos + .5f * dt_timestep * p->speed;
        p->speed = p->speed + (dt_timestep * p->force_density/p->rho);
        p->pos = p->pos + .5f * dt_timestep * p->speed;
        
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
    for(int x = p->i - 1; x <= p->i + 1; x++) {
        for(int y = p->j - 1; y <= p->j + 1; y++) {
            for(int z = p->k - 1; z <= p->k + 1; z++) {
                int idx = usg(x,y,z);

                if(idx > 0 && idx < usg.cells.size() - 1 ) {
                    // DEBUGGED / LEARNING - check if none of my neighbors have any particles - THIS IS BAD for SPH!
                    if (neighbors.size() > 40) {
                        break;
                    }
                    neighbors.insert(neighbors.end(), usg.cells.at(idx).begin(), usg.cells.at(idx).end()); //insert the particles in the cell at the end of the neighbors vector
                }
            }
        }
    }
//    if (neighbors.size() < 2) {
//       // std::cout<<"OH NO!";
//        // what should I be doing if there's not enough neighbors
//        // may be they make sure that I am moved to a nearest cell with neighbors
//        
//        // or Why am I getting into a situation like this
//        // may be parameter tuning
//           // what parameter affects how many neighbors I have?
//           // particle separation size
//            // ->
//            // grid cell size
//    }
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
//            if(rho >= d_rest_density + 100) {
//                break;
//            }
            n->a = 1;
            n->r = 0;
            n->g = 225;
            n->b = 0;
            float kernel = poly6_kernel(p->pos, n->pos);
            rho += n->mass*kernel;
            
        }
    
////        std::cout << "RHO: " << rho << std::endl;
//        if (rho < 0.2*d_rest_density) {
//            std::cout<<"WHY";
//        }

        if (fabs(rho) > d_rest_density){
//            std::cout << "Returning RHO as is" << std::endl;
            return rho;
        }
//    std::cout << "Returning RHO as REST DENSITY" << std::endl;
    return d_rest_density + rho;
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
