// #define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // 创建窗口对象
  auto window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // 初始化glad 用于调用openGL函数
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // 视口
  glViewport(0, 0, 800, 600);

  // 设置窗口变化时的回调
  glfwSetFramebufferSizeCallback(
      window, [](GLFWwindow* window, int w, int h) { glViewport(0, 0, w, h); });
  // 渲染循环
  while (!glfwWindowShouldClose(window)) {
    // 绘制
    glfwSwapBuffers(window);
    // 检查事件触发 更新窗口状态 调用对应的回调函数
    glfwPollEvents();
  }

  // 释放资源
  glfwTerminate();
}
