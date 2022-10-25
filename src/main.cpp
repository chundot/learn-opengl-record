// #define GLFW_INCLUDE_NONE
#include "utils/shader.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <tuple>
#include "utils/camera.hpp"
#include "utils/misc.hpp"
#include "utils/time.hpp"
#include "painters/light_patiner.hpp"

void processInput(GLFWwindow *window);

GLfloat Time::deltaTime = 0, Time::lastFrame = 0;
Camera *Camera::main;

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
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwMakeContextCurrent(window);

  // 初始化glad 用于调用openGL函数
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // 视口
  glViewport(0, 0, 800, 600);
  // 启用深度测试
  glEnable(GL_DEPTH_TEST);
  // 设置窗口变化时的回调
  glfwSetFramebufferSizeCallback(
      window, [](GLFWwindow *window, int w, int h) { glViewport(0, 0, w, h); });
  // 相机初始化
  Camera camera;
  // 设置鼠标移动的回调
  glfwSetCursorPosCallback(window, Camera::main->setCursorPosCallback);
  // 滚轮回调绑定
  glfwSetScrollCallback(window, Camera::main->mouseScrollCallback);
  LightPainter paint;
  paint.updateTrans();
  // 渲染循环
  while (!glfwWindowShouldClose(window)) {
    // 计算时间差
    Time::update();
    // cubePositions[5].x = cos(curFrame) * r,
    // cubePositions[5].z = sin(curFrame) * r;
    // 处理输入
    processInput(window);
    // 颜色
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 渲染
    paint.onRender();
    // 绘制
    glfwSwapBuffers(window);
    // 检查事件触发 更新窗口状态 调用对应的回调函数
    glfwPollEvents();
  }
  paint.terminate();
  // 释放资源
  glfwTerminate();
}

void processInput(GLFWwindow *window) {
  // ESC时退出
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  // 移动相机
  float cameraSpeed = 2.5f * Time::deltaTime;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    Camera::main->moveFront(cameraSpeed);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    Camera::main->moveFront(-cameraSpeed);
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    Camera::main->moveUp(cameraSpeed);
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    Camera::main->moveUp(-cameraSpeed);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    Camera::main->moveRight(-cameraSpeed);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    Camera::main->moveRight(cameraSpeed);
}