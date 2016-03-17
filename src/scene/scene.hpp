//
//  scene.hpp
//  Thanda

#include "../geom/triangle.hpp"
#include "../geom/container.hpp"
#include "../shader/shader.hpp"
#include "../camera/camera.hpp"
#include "../fluidSolver/fluidSolver.hpp"

class scene{
public:
    scene();
    ~scene();
    container* c;
    camera* scene_camera;
    fluidSolver* fluid_solver;

//    GLuint CameraRight_worldspace_ID;
//    GLuint CameraUp_worldspace_ID;

    void drawScene(GLFWwindow* window);
    void destroyScene();
    void initScene();
    GLuint vertexbuffer;
    GLuint indexbuffer;
    shader shade;

    GLuint prog_particle;
    GLuint prog_wire;

    GLuint MatrixID;
    GLuint wireMatID;

    float delta = .05;
    void keyPressEvent(GLFWwindow* window);
    void loadJSON(const char* file);


};
