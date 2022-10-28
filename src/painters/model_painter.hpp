﻿#ifndef MODEL_PAINTER_HPP
#define MODEL_PAINTER_HPP
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <string>
#include <vector>
#include <list>
#include "painter.hpp"
#include "../utils/shader.hpp"
#include "../utils/camera.hpp"
#include "../utils/imgui.hpp"
#include "../utils/nfd.hpp"
#include "../utils/model.hpp"
#include "../utils/cube.hpp"
#include "../utils/plane.hpp"
struct ModelInfo {
  std::shared_ptr<IModel> model;
  Shader *shader;
  glm::vec3 pos, scale;
  // 描边
  bool outlined;
  glm::vec3 outlineColor;
  ModelInfo(IModel *model, Shader &shader)
      : model(model),
        shader(&shader),
        pos(0, -.5f, 0),
        scale(.1f),
        outlined(false),
        outlineColor(.4f, .7f, .4f){};
};
class ModelPainter : public Painter {
 public:
  ModelPainter()
      : defShader("../../shaders/shader.vs", "../../shaders/shader.fs"),
        singleColorShader("../../shaders/shader.vs", "../../shaders/light.fs",
                          false),
        simpleShader("../../shaders/shader.vs", "../../shaders/simple.fs") {}
  virtual void init() override {
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);
    // 启用模板测试
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    // 混合及混合函数
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  virtual void terminate() override {}
  virtual void onRender() override {
    auto camera = *Camera::main;
    auto [_, view, proj] = camera.getMats();
    auto model = glm::mat4(1);
    model = glm::translate(model, glm::vec3(0, -.5f, 0));
    model = glm::scale(model, glm::vec3(.1f, .1f, .1f));
    if (sorted.size() != modelLoaded.size() || shouldSort) {
      std::unordered_map<int, float> d;
      sorted.resize(modelLoaded.size());
      std::iota(sorted.begin(), sorted.end(), 0);
      std::sort(sorted.begin(), sorted.end(), [&](int a, int b) -> bool {
        if (d.find(a) == d.end())
          d[a] = glm::length(camera.pos - modelLoaded[a].pos);
        if (d.find(b) == d.end())
          d[b] = glm::length(camera.pos - modelLoaded[b].pos);
        return d[a] > d[b];
      });
    }
    for (int i = 0; i < sorted.size(); ++i) {
      bool flag = false;
      auto cur = modelLoaded[sorted[i]];
      cur.shader->use()
          ->setTrans(glm::value_ptr(model), glm::value_ptr(view),
                     glm::value_ptr(proj))
          ->setU("numPointLights", (int)pointLights.size())
          ->setF3("viewPos", glm::value_ptr(camera.pos))
          ->setU("material.shininess", 64.0f)
          ->setPointLights(pointLights, (GLint)pointLights.size());
      updDirLight(cur.shader), updSpotLight(cur.shader);
      model = glm::mat4(1);
      model = glm::translate(model, cur.pos),
      model = glm::scale(model, cur.scale);
      // TODO 修复渲染顺序导致的边框覆盖问题
      if (cur.outlined) {
        glStencilFunc(GL_ALWAYS, 1, 0xFF);  // 所有的片段都应该更新模板缓冲
        glStencilMask(0xFF);                // 启用模板缓冲写入
        flag = true;
      } else {
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
      }
      cur.shader->use()->setMat4("model", glm::value_ptr(model));
      cur.model->Draw(*cur.shader);
      if (cur.outlined && flag) {
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);  // 禁止模板缓冲的写入
        glDisable(GL_DEPTH_TEST);
        auto delta = glm::translate(glm::mat4(1), cur.pos);
        delta = glm::scale(delta, 1.01f * cur.scale);
        singleColorShader.use()
            ->setTrans(glm::value_ptr(delta), glm::value_ptr(view),
                       glm::value_ptr(proj))
            ->setF3("objectColor", glm::value_ptr(cur.outlineColor));
        cur.model->Draw(singleColorShader);
      }
      glStencilMask(0xFF);
      glEnable(GL_DEPTH_TEST);
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
            modelLoaded.push_back({new Model(outPath), defShader});
            // TODO: 分配着色器
            free(outPath);
          } else if (res == NFD_CANCEL)
            std::cout << "User pressed cancel." << std::endl;
          else
            std::cout << "Error: " << NFD_GetError() << std::endl;
        }
        if (ImGui::MenuItem("Add Cube", "")) {
          modelLoaded.push_back(
              {new Cube("../../images/container2.png",
                        "../../images/container2_specular.png"),
               defShader});
        }
        if (ImGui::MenuItem("Add Plane", "")) {
          modelLoaded.push_back(
              {new Plane("../../images/metal.png", ""), defShader});
        }
        if (ImGui::MenuItem("Add Grass", "")) {
          modelLoaded.push_back(
              {new Plane("../../images/grass.png", "", 1), simpleShader});
        }
        if (ImGui::MenuItem("Add Window", "")) {
          modelLoaded.push_back(
              {new Plane("../../images/blending_transparent_window.png", "", 1),
               simpleShader});
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
        char id = (int)(it - modelLoaded.begin()) + '0';
        s.push_back(id);
        ImGui::BulletText("%s", s.c_str());
        if (ImGui::DragFloat3((std::string("pos##") + id).c_str(),
                              glm::value_ptr(it->pos), .1f, -15, 15))
          shouldSort = true;
        ImGui::Checkbox((std::string("Lock Ratio##") + id).c_str(), &lockRatio);
        ImGui::DragFloat3((std::string("scale##") + id).c_str(),
                          glm::value_ptr(it->scale), .01f, -15, 15);
        if (lockRatio) {
          it->scale.y = it->scale.z = it->scale.x;
        }
        ImGui::Checkbox((std::string("Outline##") + id).c_str(), &it->outlined);
        if (it->outlined) {
          ImGui::ColorEdit3((std::string("Outline Color##") + id).c_str(),
                            glm::value_ptr(it->outlineColor));
        }
        s.pop_back();
      }
    }
    ImGui::End();
  }
  void updDirLight(Shader *shader) {
    shader->setU("enableDirLight", enableDirLight)
        ->setF3("dirLight.direction", glm::value_ptr(dirLightDir))
        ->setU("dirLight.ambient", 0.3f, 0.3f, 0.3f)
        ->setU("dirLight.diffuse", 0.4f, 0.4f, 0.4f)
        ->setU("dirLight.specular", 0.5f, 0.5f, 0.5f);
  }
  void updSpotLight(Shader *shader) {
    auto camera = *Camera::main;
    shader->setU("enableSpotLight", enableSpotLight);
    if (enableSpotLight)
      // 聚光
      shader->setF3("spotLight.position", glm::value_ptr(camera.pos))
          ->setF3("spotLight.direction", glm::value_ptr(camera.front))
          ->setU("spotLight.ambient", 0.0f, 0.0f, 0.0f)
          ->setU("spotLight.diffuse", 1.0f, 1.0f, 1.0f)
          ->setU("spotLight.specular", 1.0f, 1.0f, 1.0f)
          ->setU("spotLight.constant", 1.0f)
          ->setU("spotLight.linear", 0.09f)
          ->setU("spotLight.quadratic", 0.032f)
          ->setU("spotLight.cutOff", glm::cos(glm::radians(cutOff)))
          ->setU("spotLight.outerCutOff", glm::cos(glm::radians(outerCutOff)));
  }

 private:
  std::vector<ModelInfo> modelLoaded;
  std::vector<glm::vec3> pointLights;
  std::vector<int> sorted;
  Shader defShader, singleColorShader, simpleShader;
  bool enableSpotLight = false, enableDirLight, wdActive, lockRatio, shouldSort;
  glm::vec3 dirLightDir = glm::vec3(1);
  float cutOff = 6, outerCutOff = 10;
};
#endif