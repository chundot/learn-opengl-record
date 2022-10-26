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
    model = glm::scale(model, glm::vec3(.07f, .07f, .07f));
    objShader.use(),
        objShader.setTrans(glm::value_ptr(model), glm::value_ptr(view),
                           glm::value_ptr(proj)),
        objShader.setU("numPointLights", (int)pointLights.size()),
        objShader.setU("dirLight.direction", -0.2f, -1.0f, -0.3f),
        objShader.setU("dirLight.ambient", 0.05f, 0.05f, 0.05f),
        objShader.setU("dirLight.diffuse", 0.4f, 0.4f, 0.4f),
        objShader.setU("dirLight.specular", 0.5f, 0.5f, 0.5f),
        // 聚光
        objShader.setF3("spotLight.position", glm::value_ptr(camera.pos)),
        objShader.setF3("spotLight.direction", glm::value_ptr(camera.front)),
        objShader.setU("spotLight.ambient", 0.0f, 0.0f, 0.0f),
        objShader.setU("spotLight.diffuse", 1.0f, 1.0f, 1.0f),
        objShader.setU("spotLight.specular", 1.0f, 1.0f, 1.0f),
        objShader.setU("spotLight.constant", 1.0f),
        objShader.setU("spotLight.linear", 0.09f),
        objShader.setU("spotLight.quadratic", 0.032f),
        objShader.setU("spotLight.cutOff", glm::cos(glm::radians(12.5f))),
        objShader.setU("spotLight.outerCutOff", glm::cos(glm::radians(15.0f))),
        objShader.setPointLights(pointLights, (GLint)pointLights.size());
    for (int i = 0; i < modelLoaded.size(); ++i) {
      modelLoaded[i].Draw(objShader);
    }
  }
  virtual void onImGuiRender() {
    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Exit", "Esc")) {
        }
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Model")) {
        if (ImGui::MenuItem("Load..", "Ctrl+O")) {
          nfdchar_t *outPath = NULL;
          auto res = NFD_OpenDialog(NULL, NULL, &outPath);
          if (res == NFD_OKAY) {
            std::cout << "Success! Path: " << outPath << std::endl;
            Model model(outPath);
            modelLoaded.push_back(model);
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
    if (ImGui::Button("Add Point Light"))
      pointLights.push_back(glm::vec3(0, 0, 0));
    if (pointLights.size() > 0) {
      ImGui::BulletText("Point Light Pos");
      for (auto it = pointLights.begin(); it != pointLights.end(); ++it) {
        ImGui::DragFloat3("pos", glm::value_ptr(*it), .1f, -15, 15);
      }
    }
  }

 private:
  std::vector<Model> modelLoaded;
  std::vector<glm::vec3> pointLights;
  Shader objShader;
};
#endif