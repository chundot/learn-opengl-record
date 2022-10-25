#include <glad/glad.h>
#include <GLFW/glfw3.h>
class Time {
 public:
  static GLfloat deltaTime;
  static GLfloat lastFrame;
  static void update() {
    // 计算时间差
    float curFrame = (GLfloat)glfwGetTime();
    deltaTime = curFrame - lastFrame;
    lastFrame = curFrame;
  }
};