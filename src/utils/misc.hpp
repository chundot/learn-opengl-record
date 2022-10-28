﻿#ifndef MISC_HPP
#define MISC_HPP
#include <algorithm>
#include <iostream>
#include <tuple>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.hpp"

namespace Misc {
static GLboolean rndColor;
static GLfloat color[3] = {1, 1, 1};
static std::tuple<GLfloat, GLfloat, GLfloat> getColor() {
  if (rndColor) {
    GLfloat curFrame = (GLfloat)glfwGetTime();
    color[0] = (GLfloat)sin(curFrame * 0.7),
    color[1] = (GLfloat)sin(curFrame * 1.3),
    color[2] = (GLfloat)sin(curFrame * 2);
  }
  return std::make_tuple(color[0], color[1], color[2]);
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
static GLuint textureFromFile(const char *path, bool gamma = false) {
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }
  return textureID;
}
static GLuint textureFromFile(const char *path, const std::string &directory,
                              bool gamma = false) {
  auto filename = std::string(path);
  filename = directory + '/' + filename;

  return textureFromFile(filename.c_str());
}
}  // namespace Misc
#endif