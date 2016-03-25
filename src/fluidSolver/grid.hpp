#ifndef GRID_HPP
#define GRID_HPP
#include <glm/glm.hpp>
#include <vector>
#include "fluidSolver.hpp"

class grid {
public:
    grid(glm::ivec3 dim);
    grid();
    int operator() (int i, int j, int k);
    glm::vec3 dimensions;
    std::vector<std::vector<Particle*>> particles;
};



#endif // GRID_HPP

