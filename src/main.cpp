// #define GLFW_INCLUDE_NONE
#include "utils/shader.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <tuple>
#include "utils/camera.hpp"
#include "utils/misc.hpp"
#include "utils/time.hpp"
#include "painters/light_painter.hpp"
#include "painters/model_painter.hpp"
#include "utils/imgui.hpp"

void processInput(GLFWwindow *window);

GLfloat Time::deltaTime = 0, Time::lastFrame = 0;
Camera *Camera::main;
Painter *paint;
int width = 1024, height = 768;

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);
  // 创建窗口对象
  auto window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
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
  glViewport(0, 0, width, height);
  // 设置窗口变化时的回调
  glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int w, int h) {
    glViewport(0, 0, w, h);
    if (paint) paint->setFrameSize(w, h);
  });
  // 相机初始化
  Camera camera;
  // 设置鼠标移动的回调
  glfwSetCursorPosCallback(window, Camera::main->setCursorPosCallback);
  // 滚轮回调绑定
  glfwSetScrollCallback(window, Camera::main->mouseScrollCallback);
  paint = new ModelPainter();
  paint->setFrameSize(width, height);
  paint->init();
  Gui::onInit(window);
  // 渲染循环
  while (!glfwWindowShouldClose(window)) {
    // 计算时间差
    Time::update();
    // cubePositions[5].x = cos(curFrame) * r,
    // cubePositions[5].z = sin(curFrame) * r;
    // 处理输入
    processInput(window);
    // 渲染
    paint->onRender();
    Gui::onRender(paint);
    // 绘制
    glfwSwapBuffers(window);
    // 检查事件触发 更新窗口状态 调用对应的回调函数
    glfwPollEvents();
  }
  // 释放ImGui资源
  Gui::onShutDown();
  paint->terminate();
  delete paint;
  // 释放资源
  glfwTerminate();
}

void processInput(GLFWwindow *window) {
  static auto recover = [&window]() {
    Camera::main->rotating = false;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  };
  // ESC时退出
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  // ImGUI阻塞输入
  if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow)) {
    recover();
    return;
  }
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
  // 转向
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
    Camera::main->rotating = true;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  } else {
    recover();
  }
}