#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "stb_image.hpp"
#include "glm.hpp"

class Shader {
 public:
  bool hasTexture, needSkybox;
  // 构造器读取并构建着色器
  Shader(const GLchar *vertexPath, const GLchar *fragmentPath,
         bool hasTexture = true, bool needSkybox = false)
      : hasTexture(hasTexture), needSkybox(needSkybox) {
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
  GLuint id, texId;
  // 使用程序
  Shader *use() {
    glUseProgram(id);
    return this;
  }
  // uniform工具
  Shader *setU(const std::string &name, bool value) {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
    return this;
  }
  Shader *setU(const std::string &name, int value) {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
    return this;
  }
  Shader *setU(const std::string &name, float value) {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
    return this;
  }
  Shader *setU(const std::string &name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
    return this;
  }
  Shader *setU(const std::string &name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
    return this;
  }
  Shader *setF3(const std::string &name, float *v) {
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, v);
    return this;
  }
  Shader *setMat4(const std::string &name, float *m) {
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, m);
    return this;
  }
  Shader *setSkybox() {
    if (needSkybox) {
      glActiveTexture(GL_TEXTURE3);
      glBindTexture(GL_TEXTURE_CUBE_MAP, texId);
      glActiveTexture(GL_TEXTURE0);
    }
    return this;
  }
  void BindUBlock() {
    glUniformBlockBinding(id, glGetUniformBlockIndex(id, "Matrices"), 0);
  }
  void setPointLights(glm::vec3 pos[], GLint size) {
    std::string s;
    int i = 0;
    while (i < size) {
      std::ostringstream oss;
      oss << "pointLights[" << (GLchar)('0' + i++) << ']';
      s = oss.str();
      setF3(s + ".position", glm::value_ptr(pos[i - 1]));
      setU(s + ".ambient", 0.05f, 0.05f, 0.05f);
      setU(s + ".diffuse", 0.8f, 0.8f, 0.8f);
      setU(s + ".specular", 1.0f, 1.0f, 1.0f);
      setU(s + ".constant", 1.0f);
      setU(s + ".linear", 0.09f);
      setU(s + ".quadratic", 0.032f);
    }
  }
  void setPointLights(std::vector<glm::vec3> pos, GLint size) {
    for (auto it = pos.begin(); it != pos.end(); ++it) {
      auto p = *it;
      std::string s;
      std::ostringstream oss;
      oss << "pointLights[" << (GLchar)('0' + (it - pos.begin())) << ']';
      s = oss.str();
      setF3(s + ".position", glm::value_ptr(p));
      setU(s + ".ambient", 0.1f, 0.1f, 0.1f);
      setU(s + ".diffuse", 0.8f, 0.8f, 0.8f);
      setU(s + ".specular", 1.0f, 1.0f, 1.0f);
      setU(s + ".constant", 1.0f);
      setU(s + ".linear", 0.09f);
      setU(s + ".quadratic", 0.032f);
    }
  }
  Shader *setTrans(float *model, float *view, float *proj) {
    setMat4("model", model);
    setMat4("view", view);
    setMat4("proj", proj);
    return this;
  }
};
#endif