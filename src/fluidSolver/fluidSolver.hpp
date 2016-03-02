//
//  fluidSolver.hpp
//  Thanda


#ifndef fluidSolver_hpp
#define fluidSolver_hpp
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>
#include "glm/gtx/string_cast.hpp"

struct Particle {
    glm::vec3 pos;
    glm::vec3 speed;
    unsigned char r,g,b,a; //color
    float size, angle, weight;
    float life; //remaining life of particle
    float cameradistance; //squared distnace to camera
    bool operator<(const Particle& that) const {
        //sort in reverse order, far particles drawn first
        return this->cameradistance > that.cameradistance;
    }
};

class fluidSolver {
public:
    int FindUnusedParticle();

    int ParticlesCount;
    GLuint billboard_vertex_buffer;
    GLuint particles_position_buffer;
    GLuint particles_color_buffer;

    void setParticleBuffers();
    void updateParticleBuffers();
    void drawParticles();
    void destroy();

    void sortParticles();

    void updateParticles(float delta, glm::vec3 CameraPosition);

    GLuint particlevertexbuffer;
    GLuint particleindexbuffer;
    GLuint particlecolorbuffer;

    GLint unifViewProj;
    GLint attrPos;
    GLint attrCol;

};

#endif /* fluidSolver_hpp */
