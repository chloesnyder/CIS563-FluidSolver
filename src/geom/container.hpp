#include "geom.hpp"

class container : public geom {
public:

    container();
    float minX, maxX, minY, maxY, minZ, maxZ;

//    void update();
//    void draw() const;
//    virtual void setViewProj(const float* viewProj);

    void create();
    void destroy();

    GLuint vertexbuffer;
    GLuint indexbuffer;
    GLuint colorbuffer;

    GLint unifViewProj;
    GLint attrPos;
    GLint attrCol;

};
