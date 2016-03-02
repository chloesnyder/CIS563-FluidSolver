#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>
#include "glm/gtx/string_cast.hpp"

class particle {
    static const GLfloat* g_vertex_buffer_data_array;

public:




    int ParticlesCount;
    GLuint billboard_vertex_buffer;
    GLuint particles_position_buffer;
    GLuint particles_color_buffer;

    void setParticleBuffers();
    void updateParticleBuffers();
    void drawParticles();
    void destroy();


};
