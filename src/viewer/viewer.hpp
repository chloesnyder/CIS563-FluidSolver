//
//  viewer.hpp
//  Thanda

#include "../scene/scene.hpp"



#ifndef viewer_hpp
#define viewer_hpp

class viewer {
   public:
   viewer();
   ~viewer();
   int loadWindow();

   GLFWwindow* window;
   scene* sc = new scene();
   GLuint VertexArrayID;
};



#endif /* viewer_hpp */
