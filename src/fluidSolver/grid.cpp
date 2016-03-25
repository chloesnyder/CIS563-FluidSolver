#include "grid.hpp"

grid::grid(glm::ivec3 dim)
{
    dimensions = dim;
}
grid::grid() {

}

int grid::operator() (int i, int j, int k) {
    //grid indexing method
    return (dimensions.z*dimensions.z*k) + (dimensions.y*j) + i;
}

