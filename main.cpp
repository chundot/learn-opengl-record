// #define GLFW_INCLUDE_NONE
#include "shader.hpp"
#include <cstdlib>
#include <iostream>
#include "stb_image.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void processInput(GLFWwindow *window);

float mixValue = .2f;

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

  GLfloat vertices[] = {
      // 位置                //颜色             //纹理坐标
      0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // 右上
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // 右下
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // 左下
      -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // 左上
  };

  GLuint indices[] = {
      // 索引从0开始
      0, 1, 3,  // 第一个三角形
      1, 2, 3   // 第二个三角形
  };

  // 着色器
  Shader myShader("../../shaders/shader.vs", "../../shaders/shader.fs");

  // 初始化
  GLuint VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glBindVertexArray(VAO);
  // 顶点数组复制到缓冲中
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // 索引缓冲
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);
  // 解析顶点数据
  // 位置属性
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // 颜色属性
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // 纹理映射
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // 生成纹理对象
  GLuint texture0, texture1;
  auto genTex = [](GLuint &tex, const GLchar *imgPath, GLint mode,
                   GLboolean flip = false) {
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
  };
  genTex(texture0, "../../images/container.jpg", GL_RGB);
  genTex(texture1, "../../images/awesomeface.png", GL_RGBA, true);

  myShader.use();
  myShader.setU("ourTexture0", 0), myShader.setU("ourTexture1", 1);
  // 渲染循环
  while (!glfwWindowShouldClose(window)) {
    // 处理输入
    processInput(window);
    // 颜色
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // 使用着色器程序绘制
    myShader.use();
    // myShader.setFloat("offset", .5f);
    myShader.setU("mixValue", mixValue);
    glm::mat4 model(1), view(1), proj(1);
    // 变换到世界坐标
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1, 0, 0));
    // 观察
    view = glm::translate(view, glm::vec3(0, 0, -3));
    // 透视投影
    proj = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, .1f, 100.0f);
    myShader.setU("model", glm::value_ptr(model));
    myShader.setU("view", glm::value_ptr(view));
    myShader.setU("proj", glm::value_ptr(proj));
    // 变色
    GLint vertexColorLocation = glGetUniformLocation(myShader.id, "ourColor");
    GLfloat greenVal = (GLfloat)sin(glfwGetTime()) / 2.0f + .5f;
    glUniform4f(vertexColorLocation, 0.0f, greenVal, 0.0f, 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // 绘制
    glfwSwapBuffers(window);
    // 检查事件触发 更新窗口状态 调用对应的回调函数
    glfwPollEvents();
  }

  glDeleteProgram(myShader.id);

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  // 释放资源
  glfwTerminate();
}

void processInput(GLFWwindow *window) {
  // ESC时退出
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  // 修改纹理可见度
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) mixValue += .001f;
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) mixValue -= .001f;
  mixValue = max(0.0f, min(1.0f, mixValue));
}