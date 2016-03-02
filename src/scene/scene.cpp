//
//  scene.cpp
//  Thanda

#include "scene.hpp"
#include <iostream>
#include <stdio.h>

scene::scene(){

}

scene::~scene(){

}

void scene::initScene(){
    //load shaders
   // programID = shade.LoadShaders("../CIS563-FluidSolver-master/src/shader/SimpleVertexShader.vertexshader","../CIS563-FluidSolver-master/src/shader/SimpleFragmentShader.fragmentshader");
    prog_wire = shade.LoadShaders("../CIS563-FluidSolver-master/src/shader/wire.vert","../CIS563-FluidSolver-master/src/shader/wire.frag");
    prog_particle = shade.LoadShaders("../CIS563-FluidSolver-master/src/shader/particle.vert","../CIS563-FluidSolver-master/src/shader/particle.frag");


    //create camera and geometry
    scene_camera = new camera();
    c = new container();
    c->create();
    fluid_solver = new fluidSolver();
    fluid_solver->setParticleBuffers();
    scene_camera->create();

    //send data to shader
    c->unifViewProj = glGetUniformLocation(prog_wire, "u_viewProj");
    c->attrPos = glGetAttribLocation(prog_wire, "v_pos");
    c->attrCol = glGetAttribLocation(prog_wire, "v_col");

    fluid_solver->unifViewProj = glGetUniformLocation(prog_particle, "u_viewProj");
    fluid_solver->attrPos = glGetAttribLocation(prog_particle, "v_pos");
    fluid_solver->attrCol = glGetAttribLocation(prog_particle, "v_col");

    std::cout << c->unifViewProj << ", " << c->attrPos << ", " << c->attrCol << std::endl;

    glUseProgram(prog_wire);
    glUniformMatrix4fv(c->unifViewProj, 1, GL_FALSE, &(scene_camera->MVP[0][0]));
    glUseProgram(prog_particle);
    glUniformMatrix4fv(fluid_solver->unifViewProj, 1, GL_FALSE, &(scene_camera->MVP[0][0]));
}

void scene::drawScene(GLFWwindow* window){

    //update camera
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
    glUniformMatrix4fv(fluid_solver->unifViewProj, 1, GL_FALSE, &(scene_camera->MVP[0][0]));
    fluid_solver->updateParticleBuffers();
    fluid_solver->drawParticles();
}

void scene::destroyScene() {
    t->destroy();
    c->destroy();
    fluid_solver->destroy();
    //glDeleteProgram(programID);
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
