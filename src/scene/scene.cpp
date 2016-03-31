//
//  scene.cpp
//  Thanda

#include "scene.hpp"
#include <iostream>
#include <stdio.h>
#include "json/json.h"
#include <sstream>
#include <string>
#include <fstream>

scene::scene(){

}

scene::~scene(){

}

void scene::initScene(){
    fluid_solver = new SPHSolver();
    t = 0; //initialize counter
    

    //load shaders
    fluid_solver->texture_file = shade.loadDDS("../CIS563-FluidSolver/src/shader/particle.DDS");
    prog_wire = shade.LoadShaders("../CIS563-FluidSolver/src/shader/wire.vert","../CIS563-FluidSolver/src/shader/wire.frag");
    prog_particle = shade.LoadShaders("../CIS563-FluidSolver/src/shader/particle.vert","../CIS563-FluidSolver/src/shader/particle.frag");


    //create camera and geometry
    scene_camera = new camera();
    c = new container();
    scene_camera->create();

    loadJSON("../CIS563-FluidSolver/src/shader/scene.json");
    c->create();
    fluid_solver->setParticleBuffers();
    fluid_solver->initParticles();
    fluid_solver->init(.10001);

    //send data to shader
    c->unifViewProj = glGetUniformLocation(prog_wire, "u_viewProj");
    c->attrPos = glGetAttribLocation(prog_wire, "v_pos");
    c->attrCol = glGetAttribLocation(prog_wire, "v_col");

    fluid_solver->unifViewProj = glGetUniformLocation(prog_particle, "u_viewProj");
    fluid_solver->attrPos = glGetAttribLocation(prog_particle, "v_pos");
    fluid_solver->attrCol = glGetAttribLocation(prog_particle, "v_col");
    fluid_solver->CameraRight_worldspace_ID  = glGetUniformLocation(prog_particle, "CameraRight_worldspace");
    fluid_solver->CameraUp_worldspace_ID  = glGetUniformLocation(prog_particle, "CameraUp_worldspace");
    fluid_solver->texture = glGetUniformLocation(prog_particle, "myTextureSampler");

    //initialize vectors for particle data
    int vector_size = 10000000;//fluid_solver->ParticlesContainer.size() * 4 + 4;
    fluid_solver->g_particule_position_size_data = std::vector<GLfloat>(vector_size);
    fluid_solver->g_particule_color_data = std::vector<GLubyte>(vector_size);

    glUseProgram(prog_wire);
    glUniformMatrix4fv(c->unifViewProj, 1, GL_FALSE, &(scene_camera->MVP[0][0]));
    glUseProgram(prog_particle);
    glUniformMatrix4fv(fluid_solver->unifViewProj, 1, GL_FALSE, &(scene_camera->MVP[0][0]));
}

void scene::drawScene(GLFWwindow* window){

    //update camera
    scene_camera->create();
    glUseProgram(prog_wire);
    glUniformMatrix4fv(c->unifViewProj, 1, GL_FALSE, &(scene_camera->MVP[0][0]));
    keyPressEvent(window);

    //draw container
    glBindBuffer(GL_ARRAY_BUFFER, c->vertexbuffer);
    glEnableVertexAttribArray(c->attrPos);
    glVertexAttribPointer(c->attrPos, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, c->colorbuffer);
    glEnableVertexAttribArray(c->attrCol);
    glVertexAttribPointer(c->attrCol, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c->indexbuffer);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, (void*)0);

    glDisableVertexAttribArray(c->attrPos);
    glDisableVertexAttribArray(c->attrCol);

    //draw particles
    glUseProgram(prog_particle);
    glm::vec3 CameraPosition(glm::inverse(scene_camera->viewMatrix)[3]);
    
    //update all the particles (pos, velocity, neighbors, etc)
    fluid_solver->update();

    fluid_solver->updateParticleBuffers();
    fluid_solver->ParticlesCount = 0;
    for(int i=0; i<fluid_solver->ParticlesContainer.size(); i++){

        Particle* p = fluid_solver->ParticlesContainer[i]; // shortcut
        
        p->cameradistance = glm::length2( p->pos - CameraPosition );

        // Fill the GPU buffer
        fluid_solver->g_particule_position_size_data[4*i+0] = p->pos.x;
        fluid_solver->g_particule_position_size_data[4*i+1] =  p->pos.y;
        fluid_solver->g_particule_position_size_data[4*i+2] = p->pos.z;
        fluid_solver->g_particule_position_size_data[4*i+3] = 0.05f/*p.size*/;
        
        fluid_solver->g_particule_color_data[4*i+0] = p->r;
        fluid_solver->g_particule_color_data[4*i+1] = p->g;
        fluid_solver->g_particule_color_data[4*i+2] = p->b;
        fluid_solver->g_particule_color_data[4*i+3] = 250;

        fluid_solver->ParticlesCount = fluid_solver->ParticlesContainer.size();
    }
//    std::cout << fluid_solver->ParticlesContainer.size() << std::endl;
    
    

//    //UNIT TESTING
//    std::cout << "poly6 kernel: " << fluid_solver->poly6_kernel(glm::vec3(0,0,0), glm::vec3(.1,.1,.1)) << std::endl;
//    std::cout << "spiky kernel: " << fluid_solver->spiky_kernel(glm::vec3(0,0,0), glm::vec3(.1,.1,.1)) << std::endl;
//    fluid_solver->neighborSearchUSG(fluid_solver->ParticlesContainer[4]);
//

    //fluid_solver->updateParticles(.5, CameraPosition);
    fluid_solver->updateParticleBuffers();

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fluid_solver->texture_file);
    // Set our "myTextureSampler" sampler to user Texture Unit 0
    glUniform1i(fluid_solver->texture, 0);

    // Same as the billboards tutorial
    glUniform3f(fluid_solver->CameraRight_worldspace_ID, scene_camera->viewMatrix[0][0], scene_camera->viewMatrix[1][0], scene_camera->viewMatrix[2][0]);
    glUniform3f(fluid_solver->CameraUp_worldspace_ID, scene_camera->viewMatrix[0][1], scene_camera->viewMatrix[1][1], scene_camera->viewMatrix[2][1]);

    glUniformMatrix4fv(fluid_solver->unifViewProj, 1, GL_FALSE, &(scene_camera->MVP[0][0]));
    fluid_solver->drawParticles();
}

void scene::destroyScene() {
    c->destroy();
    fluid_solver->destroy();
    delete fluid_solver;
    delete scene_camera;
    glDeleteProgram(prog_wire);
    glDeleteProgram(prog_particle);
}

void scene::keyPressEvent(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
         scene_camera->phi -= .50f;// * camera::DEG2RAD;
    } else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        scene_camera->phi += .50f;// * camera::DEG2RAD;
    } else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        scene_camera->theta -= .50f;// * camera::DEG2RAD;
    } else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        scene_camera->theta += .50f;// * camera::DEG2RAD;
    } else if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
        scene_camera->zoom -= .50f;
    } else if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
        scene_camera->zoom += .50f;
    } else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        scene_camera->fovy += .50f * camera::DEG2RAD;
    } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        scene_camera->fovy -= .50f * camera::DEG2RAD;
    } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        scene_camera->ref += glm::vec4(.25,0,0,0);
    } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        scene_camera->ref -= glm::vec4(.25,0,0,0);
    } else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        scene_camera->ref += glm::vec4(0,.25,0,0);
    } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        scene_camera->ref -= glm::vec4(0,.25,0,0);
    } else if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        scene_camera->ref -= glm::vec4(0,0,0,.25);
    } else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        scene_camera->ref += glm::vec4(0,0,0,.25);
    }

    scene_camera->RecomputeEye();
    scene_camera->create();
}

void scene::loadJSON(const char* filepath) {
    //http://jsoncpp.sourceforge.net/old.html
    Json::Value root;
    Json::Reader reader;
    std::string config_data;
    std::ifstream stream(filepath, std::ios::in);
    if(stream.is_open()) {
        std::string line = "";
        while (getline(stream, line)) {
            config_data += line;
        }
        stream.close();
    } else {
        std::cout << "failed to open" << std::endl;
    }


    bool parsingSuccessful = reader.parse(config_data.c_str(), root);
    if(!parsingSuccessful) {
        // report to the user the failure and their locations in the document.
            std::cout  << "Failed to parse configuration\n"
                       << reader.getFormattedErrorMessages();
            return;
    }

    float particleSeparation = root["particleSeparation"].asFloat();
    float scaleX = root["containerDim"]["scaleX"].asFloat();
    float scaleY = root["containerDim"]["scaleY"].asFloat();
    float scaleZ = root["containerDim"]["scaleZ"].asFloat();

    float boundX = root["particleDim"]["boundX"].asFloat();
    float boundY = root["particleDim"]["boundY"].asFloat();
    float boundZ = root["particleDim"]["boundZ"].asFloat();

    
    c->maxX = scaleX;
    c->maxY = scaleY;
    c->maxZ = scaleZ;
    c->minX = 0;
    c->minY = 0;
    c->minZ = 0;

    fluid_solver->particle_separation = particleSeparation;
    fluid_solver->boundX = boundX;
    fluid_solver->boundY = boundY;
    fluid_solver->boundZ = boundZ;
}
