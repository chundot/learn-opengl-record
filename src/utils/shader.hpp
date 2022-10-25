﻿#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <stb_image.h>

class Shader {
 public:
  // 构造器读取并构建着色器
  Shader(const GLchar *vertexPath, const GLchar *fragmentPath) {
    std::ifstream vShaderFile, fShaderFile;
    std::string vertexCode, fragmentCode;
    try {
      // 从文件路径中获取着色器
      vShaderFile.open(vertexPath), fShaderFile.open(fragmentPath);
      std::stringstream vShaderStream, fShaderStream;
      //
      vShaderStream << vShaderFile.rdbuf(),
          fShaderStream << fShaderFile.rdbuf();
      // 关闭文件
      vShaderFile.close(), fShaderFile.close();
      // 数据流转换
      vertexCode = vShaderStream.str(), fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure e) {
      std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char *vShaderCode = vertexCode.c_str(),
               *fShaderCode = fragmentCode.c_str();
    GLuint vertex, fragment;
    GLint success;
    GLchar infoLog[512];
    // 顶点
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // 错误
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(vertex, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                << infoLog << std::endl;
    }
    // 片段
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // 错误
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(fragment, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                << infoLog << std::endl;
    }
    // 链接
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    // 打印连接错误（如果有的话）
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(id, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                << infoLog << std::endl;
    }
    // 删除着色器
    glDeleteShader(vertex);
    glDeleteShader(fragment);
  }
  // 程序ID
  GLuint id;
  // 使用程序
  void use() { glUseProgram(id); }
  // uniform工具
  void setU(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
  }
  void setU(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
  }
  void setU(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
  }
  void setU(const std::string &name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
  }
  void setU(const std::string &name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
  }
  void setU(const std::string &name, float *m) const {
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, m);
  }
  void setTrans(float *model, float *view, float *proj) {
    setU("model", model);
    setU("view", view);
    setU("proj", proj);
  }
};
#endif