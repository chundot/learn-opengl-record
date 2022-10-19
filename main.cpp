// #define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void processInput(GLFWwindow *window);

// 片段着色器源码
const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";
const char *fragmentYelloShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.8f, 0.0f, 1.0f);\n"
    "}\n\0";

// 顶点着色器源代码
const GLchar *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

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
      window, [](GLFWwindow *window, int w, int h) { glViewport(0, 0, w, h); });

  GLfloat vertices[2][9] = {{0, 0, 0, .25f, .25f, 0, .5f, 0, 0},
                            {0, 0, 0, -.25f, .25f, 0, -.5f, 0, 0}};

  // 编译着色器
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  // 编译片段着色器
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  GLuint fragmentYelloShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  glShaderSource(fragmentYelloShader, 1, &fragmentYelloShaderSource, NULL);
  glCompileShader(fragmentYelloShader);

  // 创建着色器程序并附加着色器
  GLuint shaderProgram = glCreateProgram();
  GLuint shaderYelloProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderYelloProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glAttachShader(shaderYelloProgram, fragmentYelloShader);
  // 链接对象
  glLinkProgram(shaderProgram);
  glLinkProgram(shaderYelloProgram);

  // 删除着色器
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  glDeleteShader(fragmentYelloShader);

  // 初始化
  GLuint VAO[2], VBO[2], EBO;
  glGenVertexArrays(2, VAO);
  glGenBuffers(2, VBO);
  // glGenBuffers(1, &EBO);
  for (int i = 0; i < 2; ++i) {
    // 绑定
    glBindVertexArray(VAO[i]);
    // 顶点数组复制到缓冲中
    glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[i]), vertices[i],
                 GL_STATIC_DRAW);
    // 解析顶点数据
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
  }
  // 索引缓冲
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
  //              GL_STATIC_DRAW);

  // unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // 渲染循环
  while (!glfwWindowShouldClose(window)) {
    // 处理输入
    processInput(window);
    // 颜色
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // 使用着色器程序绘制
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glUseProgram(shaderYelloProgram);
    glBindVertexArray(VAO[1]);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    // 绘制
    glfwSwapBuffers(window);
    // 检查事件触发 更新窗口状态 调用对应的回调函数
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, VAO);
  glDeleteBuffers(1, VBO);
  // glDeleteBuffers(1, &EBO);
  glDeleteProgram(shaderProgram);

  // 释放资源
  glfwTerminate();
}

void processInput(GLFWwindow *window) {
  // ESC时退出
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}