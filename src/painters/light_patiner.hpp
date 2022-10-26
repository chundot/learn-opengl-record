#include <iostream>
#include "../utils/shader.hpp"
#include "../utils/camera.hpp"
#include "../utils/misc.hpp"
#include "../utils/imgui.hpp"
#include "painter.hpp"

class LightPainter : public Painter {
 public:
  LightPainter()
      : objShader("../../shaders/shader.vs", "../../shaders/shader.fs"),
        lightShader("../../shaders/shader.vs", "../../shaders/light.fs") {
    diffTex = Misc::loadTexture("../../images/container2.png", GL_RGBA, true),
    specTex = Misc::loadTexture("../../images/container2_specular.png", GL_RGBA,
                                true);
    init();
  }
  void init() override {
    initBuffer();
    objShader.use(), objShader.setU("material.diffuse", 0),
        objShader.setU("material.specular", 1);
  }
  void terminate() override {
    glDeleteProgram(objShader.id);
    glDeleteProgram(lightShader.id);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
  }
  void onRender() override {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specTex);
    glBindVertexArray(VAO);

    glm::mat4 deltaModel(1);
    // 变色
    auto [r, g, b] = Misc::getColor();
    auto camera = *Camera::main;
    auto [model, view, proj] = camera.getMats();
    // 物体
    deltaModel = glm::translate(glm::mat4(1), cubePositions[0]);
    objShader.use(), objShader.setU("objectColor", 1.0f, 0.5f, 0.31f),
        objShader.setTrans(glm::value_ptr(deltaModel), glm::value_ptr(view),
                           glm::value_ptr(proj)),
        objShader.setF3("viewPos", glm::value_ptr(camera.pos)),
        objShader.setF3("light.position", glm::value_ptr(camera.pos)),
        objShader.setF3("light.direction", glm::value_ptr(camera.front)),
        objShader.setU("light.cutOff", glm::cos(glm::radians(12.5f))),
        objShader.setU("light.outerCutOff", glm::cos(glm::radians(17.5f))),
        objShader.setU("light.ambient", 0.25f * r, 0.25f * g, 0.25f * b),
        objShader.setU("light.diffuse", 0.5f * r, 0.5f * g, 0.5f * b),
        objShader.setU("light.specular", 1.0f, 1.0f, 1.0f),
        objShader.setU("light.constant", 1.0f),
        objShader.setU("light.linear", 0.09f),
        objShader.setU("light.quadratic", 0.032f),
        objShader.setU("material.shininess", (GLfloat)shininess);
    for (unsigned int i = 0; i < 10; i++) {
      glm::mat4 model(1);
      model = glm::translate(model, cubePositions[i]);
      float angle = 20.0f * i;
      model =
          glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      objShader.setMat4("model", glm::value_ptr(model));

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    // 光源方块
    deltaModel = glm::translate(glm::mat4(1), cubePositions[10]);
    lightShader.use(), lightShader.setU("objectColor", r, g, b),
        lightShader.setTrans(glm::value_ptr(deltaModel), glm::value_ptr(view),
                             glm::value_ptr(proj));
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }
  void onImGuiRender() override {
    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */
        }
        if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */
        }
        if (ImGui::MenuItem("Close", "Ctrl+W")) {
        }
        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }
    ImGui::ColorEdit3("Light Color", Misc::color);
    ImGui::DragFloat3("Light Dir", glm::value_ptr(cubePositions[10]), .2f, -15,
                      15);
    ImGui::SliderInt("shininess", &shininess, 1, 256);
  }
  void updateTrans() {
    auto [m, v, p] = Camera::main->getMats();
    objShader.use();
    objShader.setTrans(glm::value_ptr(m), glm::value_ptr(v), glm::value_ptr(p));
  }

 private:
  GLint shininess = 64;
  Shader objShader, lightShader;
  GLuint diffTex, specTex;
  GLuint VAO, VBO, EBO;
  // 顶点大小 顶点数 面数
  GLfloat vertices[8 * 6 * 6] = {
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
      0.0f,  -1.0f, 1.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f,
      0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      0.0f,  1.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,
      -0.5f, -1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
      -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
      -1.0f, 0.0f,  1.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
      -0.5f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      1.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f};

  GLuint indices[6] = {
      // 索引从0开始
      0, 1, 3,  // 第一个三角形
      1, 2, 3   // 第二个三角形
  };
  glm::vec3 cubePositions[11] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f),
      glm::vec3(-0.2f, -1.0f, -0.3f)};
  void initBuffer() {
    // 初始化
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    // 法线映射
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 材质映射
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
  }
};