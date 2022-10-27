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
struct ModelInfo {
  Model model;
  Shader *shader;
  glm::vec3 pos, scale;
};
class ModelPainter : public Painter {
 public:
  ModelPainter()
      : defShader("../../shaders/shader.vs", "../../shaders/shader.fs") {}
  virtual void init() override {}
  virtual void terminate() override {}
  virtual void onRender() override {
    auto camera = *Camera::main;
    auto [_, view, proj] = camera.getMats();
    auto model = glm::mat4(1);
    model = glm::translate(model, glm::vec3(0, -.5f, 0));
    model = glm::scale(model, glm::vec3(.1f, .1f, .1f));
    defShader.use();
    updDirLight(), updSpotLight();
    defShader.setTrans(glm::value_ptr(model), glm::value_ptr(view),
                       glm::value_ptr(proj)),
        defShader.setU("numPointLights", (int)pointLights.size()),
        defShader.setF3("viewPos", glm::value_ptr(camera.pos)),
        defShader.setU("material.shininess", 64.0f),
        defShader.setPointLights(pointLights, (GLint)pointLights.size());
    for (int i = 0; i < modelLoaded.size(); ++i) {
      auto cur = modelLoaded[i];
      model = glm::mat4(1);
      model = glm::translate(model, cur.pos),
      model = glm::scale(model, cur.scale);
      cur.shader->use(), cur.shader->setMat4("model", glm::value_ptr(model));
      cur.model.Draw(*cur.shader);
    }
  }
  void onImGuiRender() override {
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
            modelLoaded.push_back(
                {model, &defShader, glm::vec3(0, -.5f, 0), glm::vec3(.1f)});
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
      if (ImGui::Button("Add Default Light")) {
        pointLights.emplace_back(0.7f, 0.2f, 2.0f),
            pointLights.emplace_back(2.3f, -3.3f, -4.0f),
            pointLights.emplace_back(-4.0f, 2.0f, -12.0f),
            pointLights.emplace_back(0.0f, 0.0f, -3.0f);
      }
      if (ImGui::Button("Add Point Light"))
        pointLights.push_back(glm::vec3(0, 0, 0));
      if (pointLights.size() && ImGui::CollapsingHeader("Point Light Pos")) {
        for (auto it = pointLights.begin(); it != pointLights.end(); ++it) {
          ImGui::DragFloat3("pos", glm::value_ptr(*it), .1f, -15, 15);
        }
      }
    }
    // 模型相关
    if (modelLoaded.size() && ImGui::CollapsingHeader("Models")) {
      std::string s = "Model";
      for (auto it = modelLoaded.begin(); it != modelLoaded.end(); ++it) {
        s.push_back((int)(it - modelLoaded.begin()) + '0');
        ImGui::BulletText("%s", s.c_str());
        ImGui::DragFloat3("pos", glm::value_ptr(it->pos), .1f, -15, 15);
        ImGui::Checkbox("Lock Ratio", &lockRatio);
        ImGui::DragFloat3("scale", glm::value_ptr(it->scale), .01f, -15, 15);
        if (lockRatio) {
          it->scale.y = it->scale.z = it->scale.x;
        }
        s.pop_back();
      }
    }
    ImGui::End();
  }
  void updDirLight() {
    defShader.setU("enableDirLight", enableDirLight),
        defShader.setF3("dirLight.direction", glm::value_ptr(dirLightDir)),
        defShader.setU("dirLight.ambient", 0.3f, 0.3f, 0.3f),
        defShader.setU("dirLight.diffuse", 0.4f, 0.4f, 0.4f),
        defShader.setU("dirLight.specular", 0.5f, 0.5f, 0.5f);
  }
  void updSpotLight() {
    auto camera = *Camera::main;
    defShader.setU("enableSpotLight", enableSpotLight);
    if (enableSpotLight)
      // 聚光
      defShader.setF3("spotLight.position", glm::value_ptr(camera.pos)),
          defShader.setF3("spotLight.direction", glm::value_ptr(camera.front)),
          defShader.setU("spotLight.ambient", 0.0f, 0.0f, 0.0f),
          defShader.setU("spotLight.diffuse", 1.0f, 1.0f, 1.0f),
          defShader.setU("spotLight.specular", 1.0f, 1.0f, 1.0f),
          defShader.setU("spotLight.constant", 1.0f),
          defShader.setU("spotLight.linear", 0.09f),
          defShader.setU("spotLight.quadratic", 0.032f),
          defShader.setU("spotLight.cutOff", glm::cos(glm::radians(cutOff))),
          defShader.setU("spotLight.outerCutOff",
                         glm::cos(glm::radians(outerCutOff)));
  }

 private:
  std::vector<ModelInfo> modelLoaded;
  std::vector<glm::vec3> pointLights;
  Shader defShader;
  bool enableSpotLight = false, enableDirLight, wdActive, lockRatio;
  glm::vec3 dirLightDir = glm::vec3(1);
  float cutOff = 6, outerCutOff = 10;
};
#endif