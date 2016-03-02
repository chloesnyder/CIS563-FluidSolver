#include "particle.hpp"

// The VBO containing the 4 vertices of the particles.
static const GLfloat g_vertex_buffer_data[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f,
    0.5f, 0.5f, 0.0f,
};

const int MaxParticles = 100000;
static GLfloat* g_particule_position_size_data = new GLfloat[MaxParticles * 4];
static GLubyte* g_particule_color_data         = new GLubyte[MaxParticles * 4];


