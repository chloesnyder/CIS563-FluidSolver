//
//  camera.cpp
//  Thanda
//

#include "camera.hpp"


void camera::RecomputeEye()
{
    eye = glm::rotate(glm::mat4(1.0f), theta, glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1.0f), phi, glm::vec3(1, 0, 0)) * glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, zoom)) * glm::vec4(0, 0, 0, 1);
    up = glm::rotate(glm::mat4(1.0f), theta, glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1.0f), phi, glm::vec3(1, 0, 0)) * glm::vec4(0, 1, 0, 0);
}

glm::mat4 camera::getViewProj()
{
    return glm::perspective(fovy, width / height, near_clip, far_clip) * glm::lookAt(glm::vec3(eye), glm::vec3(ref), glm::vec3(up));
}

camera::camera(){
    theta = 30;
    phi = -10;
    zoom = 10;
    fovy = 60;
    width = 500;
    height = 400;
    near_clip = 0.1f;
    far_clip = 1000;
    
    //eye = glm::vec4(1,0,0,1);

    eye = glm::rotate(glm::mat4(1.0f), theta, glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1.0f), phi, glm::vec3(1, 0, 0)) * glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, zoom)) * glm::vec4(0, 0, 0, 1);
    ref = glm::vec4(0, 0, 0, 1);
    up = glm::rotate(glm::mat4(1.0f), theta, glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1.0f), phi, glm::vec3(1, 0, 0)) * glm::vec4(0, 1, 0, 0);
}

camera::~camera(){

}

void camera::create() {
    projectionMatrix = glm::perspective((fovy),  width / height, near_clip, far_clip);
    viewMatrix = glm::lookAt(glm::vec3(eye), glm::vec3(ref), glm::vec3(up));
    modelMatrix = glm::mat4(1.0f);
    MVP = projectionMatrix* viewMatrix * modelMatrix;
}

