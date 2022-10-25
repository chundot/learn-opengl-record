#ifndef MISC_HPP
#define MISC_HPP
#include <iostream>
#include <tuple>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.hpp"

class Misc {
 public:
  static std::tuple<GLfloat, GLfloat, GLfloat> rndColor() {
    GLfloat curFrame = (GLfloat)glfwGetTime();
    return std::make_tuple((GLfloat)sin(curFrame * 0.7),
                           (GLfloat)sin(curFrame * 1.3),
                           (GLfloat)sin(curFrame * 2));
  }
  static GLuint loadTexture(const GLchar *imgPath, GLint mode,
                            GLboolean flip = false) {
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    // 设置环绕模式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载并生成纹理
    GLint width, height, nrChannels;
    stbi_set_flip_vertically_on_load(flip);
    unsigned char *data = stbi_load(imgPath, &width, &height, &nrChannels, 0);
    if (data) {
      glad_glTexImage2D(GL_TEXTURE_2D, 0, mode, width, height, 0, mode,
                        GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    } else {
      std::cout << "Failed to load texture" << std::endl;
    }
    // 释放已加载的图像
    stbi_image_free(data);
    return tex;
  }
};
#endif