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
    triangle* t;
    container* c;
    camera* scene_camera;
    fluidSolver* fluid_solver;

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

    void keyPressEvent(GLFWwindow* window);


};
