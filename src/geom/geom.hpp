//
//  geom.hpp
//  Thanda

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#ifndef geom_hpp
#define geom_hpp
class geom {
public:
    geom();
    ~geom() {}

    virtual void create() {};
    virtual void destroy() {};
    virtual void draw() {};

};


#endif /* geom_hpp */
