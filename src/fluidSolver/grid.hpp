#ifndef GRID_HPP
#define GRID_HPP
#include <glm/glm.hpp>
#include <vector>
#include "fluidSolver.hpp"
#include <tbb/tbb.h>
#include "tbb/concurrent_vector.h"
using namespace tbb;

class grid {
public:
    grid(int grid_min, int grid_max, float resolution);
    grid();
    int operator() (int i, int j, int k);
    glm::vec3 dimensions;
    concurrent_vector<concurrent_vector<Particle*>> cells;
    int grid_min;
    int grid_max;
    float resolution;
    float cell_size;
    int zIndex(int i, int j, int k);
};



#endif // GRID_HPP

