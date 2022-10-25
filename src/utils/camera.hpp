#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tuple>

struct Euler {
  float yaw, pitch, roll;
};

class Camera {
 public:
  static Camera *main;
  Camera() {
    pos = glm::vec3(0, 0, 3);
    up = glm::vec3(0, 1, 0);
    front = glm::vec3(0, 0, -1);
    right = glm::normalize(glm::cross(up, front));
    target = glm::vec3(0, 0, 0);
    euler = {0, 0, 0};
    if (!main) main = this;
  }
  Euler euler;
  float fov = 45;
  glm::vec3 pos, target;
  glm::vec3 front, right, up;
  void updateEuler(float xOffset, float yOffset) {
    euler.pitch += yOffset, euler.yaw += xOffset;
    euler.pitch = fmin(89.0f, fmax(-89.0f, euler.pitch));
    front = glm::normalize(glm::vec3(
        cos(glm::radians(euler.yaw)) * cos(glm::radians(euler.pitch)),
        sin(glm::radians(euler.pitch)),
        sin(glm::radians(euler.yaw)) * cos(glm::radians(euler.pitch))));
  }
  static void mouseScrollCallback(GLFWwindow *window, double xOffset,
                                  double yOffset) {
    Camera::main->updateFov((GLfloat)yOffset);
  }
  static void setCursorPosCallback(GLFWwindow *window, double xPos,
                                   double yPos) {
    if (main->firstMouse)
      main->lastX = (GLfloat)xPos, main->lastY = (GLfloat)yPos,
      main->firstMouse = false;
    GLfloat xOffset = (GLfloat)xPos - main->lastX,
            yOffset = main->lastY - (GLfloat)yPos;
    main->lastX = (GLfloat)xPos, main->lastY = (GLfloat)yPos;
    GLfloat sensitivity = 0.01f;
    xOffset *= sensitivity, yOffset *= sensitivity;
    main->updateEuler(xOffset, yOffset);
  }
  std::tuple<glm::mat4, glm::mat4, glm::mat4> getMats(bool getModel = true,
                                                      bool getView = true,
                                                      bool getProj = true) {
    // 变换到世界坐标
    if (getModel)
      model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1, 0, 0));
    // 观察
    if (getView) view = glm::lookAt(pos, pos + front, up);
    // 透视投影
    if (getProj)
      proj = glm::perspective(glm::radians(fov), 4.0f / 3.0f, .1f, 100.0f);
    return std::make_tuple(model, view, proj);
  }
  void moveFront(GLfloat spd) { pos += spd * (front); }
  void moveUp(GLfloat spd) { pos += spd * (up); }
  void moveRight(GLfloat spd) {
    pos += spd * (glm::normalize(glm::cross(front, up)));
  }

 private:
  glm::mat4 model, proj, view;
  bool firstMouse;
  float lastX = 400, lastY = 300;  // 鼠标位置
  void updateFov(float offset) { fov = fmin(45.0f, fmax(1.0f, fov - offset)); }
};

#endif