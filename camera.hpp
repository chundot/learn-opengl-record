#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Euler {
  float yaw, pitch, roll;
};

class Camera {
 public:
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
  void updateFov(float offset) { fov = fmin(45.0f, fmax(1.0f, fov - offset)); }
};

#endif