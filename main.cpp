// #define GLFW_INCLUDE_NONE
#include "shader.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include "stb_image.hpp"
#include "camera.hpp"

void processInput(GLFWwindow *window);

float mixValue = .2f;
float deltaTime = 0.0f;          // 当前帧与上一帧的时间差
float lastFrame = 0.0f;          // 上一帧的时间
float lastX = 400, lastY = 300;  // 鼠标位置

Camera camera;

bool firstMouse = true;

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
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwMakeContextCurrent(window);

  // 初始化glad 用于调用openGL函数
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // 视口
  glViewport(0, 0, 800, 600);
  // 启用深度测试
  glEnable(GL_DEPTH_TEST);
  // 设置窗口变化时的回调
  glfwSetFramebufferSizeCallback(
      window, [](GLFWwindow *window, int w, int h) { glViewport(0, 0, w, h); });
  GLfloat vertices[] = {
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  -0.5f, -0.5f,
      0.0f,  0.0f,  -1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, -0.5f, 0.5f,  -0.5f,
      0.0f,  0.0f,  -1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,
      0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -0.5f, 0.5f,  0.5f,
      0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f,
      -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, 0.5f,
      -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
      1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, 0.5f,
      1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, -0.5f,
      0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, 0.5f,
      0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
      0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,
      0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f};

  GLuint indices[] = {
      // 索引从0开始
      0, 1, 3,  // 第一个三角形
      1, 2, 3   // 第二个三角形
  };
  glm::vec3 cubePositions[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

  // 着色器
  Shader myShader("../../shaders/shader.vs", "../../shaders/shader.fs"),
      lightShader("../../shaders/shader.vs", "../../shaders/light.fs");

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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // 法线映射
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

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
  glm::mat4 model(1), proj(1);
  // 相机
  camera.pos = glm::vec3(0, 0, 3);
  camera.up = glm::vec3(0, 1, 0);
  camera.front = glm::vec3(0, 0, -1);
  camera.right = glm::normalize(glm::cross(camera.up, camera.front));
  camera.target = glm::vec3(0, 0, 0);
  auto view = glm::lookAt(camera.pos, camera.target, camera.up);
  // 设置鼠标移动的回调
  glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xPos,
                                      double yPos) {
    if (firstMouse)
      lastX = (GLfloat)xPos, lastY = (GLfloat)yPos, firstMouse = false;
    GLfloat xOffset = (GLfloat)xPos - lastX, yOffset = lastY - (GLfloat)yPos;
    lastX = (GLfloat)xPos, lastY = (GLfloat)yPos;
    GLfloat sensitivity = 0.01f;
    xOffset *= sensitivity, yOffset *= sensitivity;
    camera.updateEuler(xOffset, yOffset);
  });
  glfwSetScrollCallback(window,
                        [](GLFWwindow *window, double xOffset, double yOffset) {
                          camera.updateFov((GLfloat)yOffset);
                        });
  // 变换到世界坐标
  model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1, 0, 0));
  // 观察
  view = glm::translate(view, glm::vec3(0, 0, -3));
  // 透视投影
  proj = glm::perspective(glm::radians(camera.fov), 4.0f / 3.0f, .1f, 100.0f);
  myShader.setTrans(glm::value_ptr(model), glm::value_ptr(view),
                    glm::value_ptr(proj));
  GLfloat r = sqrt(cubePositions[5].x * cubePositions[5].x +
                   cubePositions[5].z * cubePositions[5].z);
  // 渲染循环
  while (!glfwWindowShouldClose(window)) {
    // 计算时间差
    float curFrame = (GLfloat)glfwGetTime();
    deltaTime = curFrame - lastFrame;
    lastFrame = curFrame;
    cubePositions[5].x = cos(curFrame) * r,
    cubePositions[5].z = sin(curFrame) * r;
    // 处理输入
    processInput(window);
    // 颜色
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glBindVertexArray(VAO);
    view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
    proj = glm::perspective(glm::radians(camera.fov), 4.0f / 3.0f, .1f, 100.0f);
    glm::mat4 deltaModel(1);
    // 物体
    deltaModel = glm::translate(glm::mat4(1), cubePositions[0]);
    myShader.use(), myShader.setU("objectColor", 1.0f, 0.5f, 0.31f),
        myShader.setU("lightColor", 1.0f, 1.0f, 1.0f),
        myShader.setTrans(glm::value_ptr(deltaModel), glm::value_ptr(view),
                          glm::value_ptr(proj)),
        myShader.setU("viewPos", camera.pos.x, camera.pos.y, camera.pos.z),
        myShader.setU("lightPos", cubePositions[5].x, cubePositions[5].y,
                      cubePositions[5].z);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // 光源
    deltaModel = glm::translate(glm::mat4(1), cubePositions[5]);
    lightShader.use(),
        lightShader.setTrans(glm::value_ptr(deltaModel), glm::value_ptr(view),
                             glm::value_ptr(proj));
    glDrawArrays(GL_TRIANGLES, 0, 36);
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
  // 移动相机
  float cameraSpeed = 2.5f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.pos += cameraSpeed * (camera.front);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.pos -= cameraSpeed * (camera.front);
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    camera.pos += cameraSpeed * (camera.up);
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    camera.pos -= cameraSpeed * (camera.up);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.pos -=
        glm::normalize(glm::cross(camera.front, camera.up)) * cameraSpeed;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.pos +=
        glm::normalize(glm::cross(camera.front, camera.up)) * cameraSpeed;
}