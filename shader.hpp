#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

class Shader {
 public:
  // 构造器读取并构建着色器
  Shader(const GLchar *vertexPath, const GLchar *fragmentPath) {
    ifstream vShaderFile, fShaderFile;
    string vertexCode, fragmentCode;
    try {
      // 从文件路径中获取着色器
      vShaderFile.open(vertexPath), fShaderFile.open(fragmentPath);
      stringstream vShaderStream, fShaderStream;
      //
      vShaderStream << vShaderFile.rdbuf(),
          fShaderStream << fShaderFile.rdbuf();
      // 关闭文件
      vShaderFile.close(), fShaderFile.close();
      // 数据流转换
      vertexCode = vShaderStream.str(), fragmentCode = fShaderStream.str();
    } catch (ifstream::failure e) {
      cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
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
      cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    }
    // 片段
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // 错误
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(fragment, 512, NULL, infoLog);
      cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
           << infoLog << endl;
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
      cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
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
  void setBool(const string &name, bool value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
  }
  void setInt(const string &name, int value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
  }
  void setFloat(const string &name, float value) const {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
  }
};
#endif