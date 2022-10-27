#ifndef MODEL_PAINTER_HPP
#define MODEL_PAINTER_HPP
#include <string>
#include <vector>
#include "painter.hpp"
#include "../utils/shader.hpp"
#include "../utils/camera.hpp"
#include "../utils/imgui.hpp"
#include "../utils/nfd.hpp"
#include "../utils/model.hpp"
class ModelPainter : public Painter {
 public:
  ModelPainter()
      : objShader("../../shaders/shader.vs", "../../shaders/shader.fs") {}
  virtual void init() {}
  virtual void terminate() {}
  virtual void onRender() {
    auto camera = *Camera::main;
    auto [_, view, proj] = camera.getMats();
    auto model = glm::mat4(1);
    model = glm::translate(model, glm::vec3(0, -.5f, 0));
    model = glm::scale(model, glm::vec3(.1f, .1f, .1f));
    objShader.use();
    updDirLight(), updSpotLight();
    objShader.setTrans(glm::value_ptr(model), glm::value_ptr(view),
                       glm::value_ptr(proj)),
        objShader.setU("numPointLights", (int)pointLights.size()),
        objShader.setU("material.shininess", 64.0f),
        objShader.setPointLights(pointLights, (GLint)pointLights.size());
    for (int i = 0; i < modelLoaded.size(); ++i) {
      modelLoaded[i].Draw(objShader);
    }
  }
  virtual void onImGuiRender() {
    ImGui::Begin("Debug Tool", &wdActive, ImGuiWindowFlags_MenuBar);
    // 顶部菜单栏
    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Exit", "Esc")) {
        }
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Model")) {
        // 加载模型
        if (ImGui::MenuItem("Load..", "Ctrl+O")) {
          nfdchar_t *outPath = NULL;
          auto res = NFD_OpenDialog(NULL, NULL, &outPath);
          if (res == NFD_OKAY) {
            std::cout << "Success! Path: " << outPath << std::endl;
            Model model(outPath);
            modelLoaded.push_back(model);
            // TODO: 分配着色器
            free(outPath);
          } else if (res == NFD_CANCEL)
            std::cout << "User pressed cancel." << std::endl;
          else
            std::cout << "Error: " << NFD_GetError() << std::endl;
        }
        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
      ImGui::End();
    }
    // 着色器
    if (ImGui::CollapsingHeader("Shader")) {
      // TODO: 添加, 删除, 编辑着色器
    }
    // 光源相关设置
    if (ImGui::CollapsingHeader("Lights")) {
      // 平行光
      ImGui::BulletText("Directional Light");
      ImGui::Checkbox("Enable Directional Light", &enableDirLight);
      if (enableDirLight) {
        ImGui::InputFloat3("Direction", glm::value_ptr(dirLightDir));
      }
      // 聚光灯
      ImGui::BulletText("SpotLight");
      ImGui::Checkbox("Enable SpotLight", &enableSpotLight);
      if (enableSpotLight) {
        ImGui::DragFloat("Cut Off", &cutOff, .5f, 0, 30);
        ImGui::DragFloat("Outer Cut Off", &outerCutOff, .5f, 1, 30);
        cutOff = fmin(cutOff, outerCutOff);
      }
      // 点光源相关
      ImGui::BulletText("Point Light");
      if (ImGui::Button("Add Point Light"))
        pointLights.push_back(glm::vec3(0, 0, 0));
      if (pointLights.size() > 0) {
        if (ImGui::CollapsingHeader("Point Light Pos")) {
          for (auto it = pointLights.begin(); it != pointLights.end(); ++it) {
            ImGui::DragFloat3("pos", glm::value_ptr(*it), .1f, -15, 15);
          }
        }
      }
    }
  }
  void updDirLight() {
    objShader.setU("enableDirLight", enableDirLight),
        objShader.setF3("dirLight.direction", glm::value_ptr(dirLightDir)),
        objShader.setU("dirLight.ambient", 0.05f, 0.05f, 0.05f),
        objShader.setU("dirLight.diffuse", 0.4f, 0.4f, 0.4f),
        objShader.setU("dirLight.specular", 0.5f, 0.5f, 0.5f);
  }
  void updSpotLight() {
    auto camera = *Camera::main;
    objShader.setU("enableSpotLight", enableSpotLight);
    if (enableSpotLight)
      // 聚光
      objShader.setF3("spotLight.position", glm::value_ptr(camera.pos)),
          objShader.setF3("spotLight.direction", glm::value_ptr(camera.front)),
          objShader.setU("spotLight.ambient", 0.0f, 0.0f, 0.0f),
          objShader.setU("spotLight.diffuse", 1.0f, 1.0f, 1.0f),
          objShader.setU("spotLight.specular", 1.0f, 1.0f, 1.0f),
          objShader.setU("spotLight.constant", 1.0f),
          objShader.setU("spotLight.linear", 0.09f),
          objShader.setU("spotLight.quadratic", 0.032f),
          objShader.setU("spotLight.cutOff", glm::cos(glm::radians(cutOff))),
          objShader.setU("spotLight.outerCutOff",
                         glm::cos(glm::radians(outerCutOff)));
  }

 private:
  std::vector<Model> modelLoaded;
  std::vector<glm::vec3> pointLights;
  Shader objShader;
  bool enableSpotLight = false, enableDirLight, wdActive;
  glm::vec3 dirLightDir = glm::vec3(1);
  float cutOff = 6, outerCutOff = 10;
};
#endif