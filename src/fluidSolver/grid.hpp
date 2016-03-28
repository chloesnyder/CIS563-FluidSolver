#ifndef GRID_HPP
#define GRID_HPP
#include <glm/glm.hpp>
#include <vector>
#include "fluidSolver.hpp"

class grid {
public:
    grid(int grid_min, int grid_max, float resolution);
    grid();
    int operator() (int i, int j, int k);
    glm::vec3 dimensions;
    std::vector<std::vector<Particle*>> particles;
    int grid_min;
    int grid_max;
    float resolution;
};



#endif // GRID_HPP

