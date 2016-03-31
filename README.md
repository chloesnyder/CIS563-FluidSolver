# CIS563-FluidSolver
(Credit : CIS565 README)

Fluid Solver Submission guidelines:


- If you have modified any of the CMakeLists.txt files at all (aside from the list of CORE_SRC), you must test that your project can build. Beware of any build issues.

- Open a GitHub pull request so that we can see that you have finished. The title should be "Submission: YOUR NAME".

- In the body of the pull request, include a link to your repository.

- Submit on canvas with a direct link to your pull request on GitHub


And you're done!

SPRINT 1:
Unit tests:
The unit tests simply print out the values, which I calculated by hand. They are commented out in the body of my drawScene loop.

The neighbor test is verified by turning the particles green. If all the particles are green then we know the neighbor test worked. Neighbor test simply figures out which particles should go into which cell and assign the appropriate grid index. The grid contains "cells", which are a std::vector of a std::vec of particles. Calling usg.cells.at(p->gridIndex) returns the vec of particles contained in that cell (including p itself). In order for p itself not to be included in calculating density and pressure, the kernel functions rely on an epsilon value so that only particles a certain distance away from p are used in the calculations.

Naive neighbor search also implemented, but not used.

Please note that Debanshu said that #defines is bad, so I did not change my deg2rad implementation: http://debian.fmi.uni-sofia.bg/~mrpaff/Effective%20C++/EC/EI1_FR.HTM

