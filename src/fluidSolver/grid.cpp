#include "grid.hpp"

grid::grid(int grid_min, int grid_max, float cell_size)
{
    this->resolution = ceil((grid_max - grid_min) / cell_size);
    cells.resize(resolution*resolution*resolution);
    this->grid_max = grid_max;
    this->grid_min = grid_min;
    this->cell_size = cell_size;
}
grid::grid() {

}

int grid::operator() (int i, int j, int k) {
    //grid indexing method
    return (resolution*resolution*k) + (resolution*j) + i;
}

