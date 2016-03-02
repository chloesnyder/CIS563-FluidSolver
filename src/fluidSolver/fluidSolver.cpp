//
//  fluidSolver.cpp
//  Thanda

#include "fluidSolver.hpp"

const int MaxParticles = 100000;
static GLfloat* g_particule_position_size_data = new GLfloat[MaxParticles * 4];
static GLubyte* g_particule_color_data = new GLubyte[MaxParticles * 4];
static const GLfloat g_vertex_buffer_data[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f,
    0.5f, 0.5f, 0.0f,
};
Particle ParticlesContainer[MaxParticles];
int LastUsedParticle = 0;

// Finds a Particle in ParticlesContainer which isn't used yet.
// (i.e. life < 0);
int fluidSolver::FindUnusedParticle(){

    for(int i=LastUsedParticle; i<100000; i++){
        if (ParticlesContainer[i].life < 0){
            LastUsedParticle = i;
            return i;
        }
    }

    for(int i=0; i<LastUsedParticle; i++){
        if (ParticlesContainer[i].life < 0){
            LastUsedParticle = i;
            return i;
        }
    }

    return 0; // All particles are taken, override the first one
}

void fluidSolver::setParticleBuffers(){

    GLuint billboard_vertex_buffer;
    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // The VBO containing the positions and sizes of the particles
    GLuint particles_position_buffer;
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);

    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    // The VBO containing the colors of the particles
    GLuint particles_color_buffer;
    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
}

void fluidSolver::updateParticleBuffers() {
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);

    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void fluidSolver::drawParticles() {
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glVertexAttribPointer(
                0, // attribute. No particular reason for 0, but must match the layout in the shader.
                3, // size
                GL_FLOAT, // type
                GL_FALSE, // normalized?
                0, // stride
                (void*)0 // array buffer offset
                );

    // 2nd attribute buffer : positions of particles' centers
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glVertexAttribPointer(
                1, // attribute. No particular reason for 1, but must match the layout in the shader.
                4, // size : x + y + z + size => 4
                GL_FLOAT, // type
                GL_FALSE, // normalized?
                0, // stride
                (void*)0 // array buffer offset
                );

    // 3rd attribute buffer : particles' colors
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glVertexAttribPointer(
                2, // attribute. No particular reason for 1, but must match the layout in the shader.
                4, // size : r + g + b + a => 4
                GL_UNSIGNED_BYTE, // type
                GL_TRUE, // normalized? *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
                0, // stride
                (void*)0 // array buffer offset
                );

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void fluidSolver::destroy() {
    delete[] g_particule_position_size_data;

    // Cleanup VBO and shader
    glDeleteBuffers(1, &particles_color_buffer);
    glDeleteBuffers(1, &particles_position_buffer);
    glDeleteBuffers(1, &billboard_vertex_buffer);
}

void fluidSolver::sortParticles() {
    std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
}

void fluidSolver::updateParticles(float delta, glm::vec3 CameraPosition) {
    // Simulate all particles
    int ParticlesCount = 0;

    for(int i=0; i<MaxParticles; i++){

        Particle& p = ParticlesContainer[i]; // shortcut

        if(p.life > 0.0f){

            // Decrease life
            p.life -= delta;
            if (p.life > 0.0f){

                // Simulate simple physics : gravity only, no collisions
                p.speed += glm::vec3(0.0f,-9.81f, 0.0f) * (float)delta * 0.5f;
                p.pos += p.speed * (float)delta;
                p.cameradistance = glm::length2( p.pos - CameraPosition );
                //ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

                // Fill the GPU buffer
                g_particule_position_size_data[4*ParticlesCount+0] = p.pos.x;
                g_particule_position_size_data[4*ParticlesCount+1] = p.pos.y;
                g_particule_position_size_data[4*ParticlesCount+2] = p.pos.z;

                g_particule_position_size_data[4*ParticlesCount+3] = p.size;

                g_particule_color_data[4*ParticlesCount+0] = p.r;
                g_particule_color_data[4*ParticlesCount+1] = p.g;
                g_particule_color_data[4*ParticlesCount+2] = p.b;
                g_particule_color_data[4*ParticlesCount+3] = p.a;

            }else{
                // Particles that just died will be put at the end of the buffer in SortParticles();
                p.cameradistance = -1.0f;
            }

            ParticlesCount++;

        }
    }
}
