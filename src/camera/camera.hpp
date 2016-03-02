//
//  camera.hpp
//  Thanda
//

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>


#ifndef camera_hpp
#define camera_hpp

class camera {
public:
    constexpr static const float DEG2RAD = 3.14f / 180.f;
    camera();
    ~camera();
    void create();
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 modelMatrix;
    glm::mat4 MVP;

    glm::vec4 eye;
    glm::vec4 ref;
    glm::vec4 up;
    float width;
    float height;
    float fovy;
    float near_clip;
    float far_clip;
    float phi;
    float theta;
    float zoom;

    glm::mat4 getViewProj();
    void RecomputeEye();

};

#endif /* camera_hpp */
