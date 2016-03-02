#include "geom.hpp"

class triangle : public geom {
public:
    void create();
    void destroy();

    GLuint vertexbuffer;
    GLuint indexbuffer;
    GLuint colorbuffer;

    GLint unifViewProj;
    GLint attrPos;
    GLint attrCol;
};
