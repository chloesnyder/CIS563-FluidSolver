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
    int id;
    unsigned char r,g,b,a; //color
    float size, angle, weight;
    float cameradistance; //squared distnace to camera
    bool operator<(const Particle& that) const {
        //sort in reverse order, far particles drawn first
        return this->cameradistance > that.cameradistance;
    }

    float rho; //density
    float mass = 0.125f;
    float pres;
    int gridIndex;
    glm::vec3 f_pressure;
    glm::vec3 f_gravity;
    glm::vec3 force_density;
    glm::vec3 f_visc;
    
    //3d grid indices
    int i;
    int j;
    int k;
    
    //QUESTION: Does it make more sense to store neighbors in the particle, or not?
    std::vector<Particle*> neighbors;
};

class fluidSolver {
public:

    std::vector<GLfloat> g_particule_position_size_data;
    std::vector<GLubyte> g_particule_color_data;
    int MaxParticles;
    std::vector<Particle*> ParticlesContainer;

    int ParticlesCount = 0;
    GLuint billboard_vertex_buffer;
    GLuint particles_position_buffer;
    GLuint particles_color_buffer;
    GLuint texture;
    GLuint texture_file;
    GLuint CameraRight_worldspace_ID;
    GLuint CameraUp_worldspace_ID;

    void setParticleBuffers();
    void updateParticleBuffers();
    void drawParticles();
    void destroy();

    void sortParticles();
    //void updateParticles(float delta, glm::vec3 CameraPosition);
    virtual void initParticles()=0;

    GLuint particlevertexbuffer;
    GLuint particleindexbuffer;
    GLuint particlecolorbuffer;

    GLint unifViewProj;
    GLint attrPos;
    GLint attrCol;

    float particle_separation;
    float boundX;
    float boundY;
    float boundZ;

};

#endif /* fluidSolver_hpp */
