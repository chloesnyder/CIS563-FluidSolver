#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "../geom/geom.hpp"

class shader {
public:
    GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
};

