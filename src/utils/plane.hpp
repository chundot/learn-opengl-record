#ifndef PLANE_HPP
#define PLANE_HPP
#include <vector>
#include "shader.hpp"
#include "mesh.hpp"
#include "misc.hpp"
#include "imodel.hpp"

class Plane : public IModel {
 public:
  Plane(const char *diffPath, const char *specPath, int typeNum = 0) {
    static std::vector<std::vector<float>> v = {
        {5.0f,  -0.5f, 5.0f,  2.0f, 0.0f, -5.0f, -0.5f, 5.0f,  0.0f, 0.0f,
         -5.0f, -0.5f, -5.0f, 0.0f, 2.0f, 5.0f,  -0.5f, -5.0f, 2.0f, 2.0f},
        {0.0f, 0.5f,  0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f,
         1.0f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.5f,  0.0f, 1.0f, 0.0f}};
    std::vector<Texture> t;
    if (diffPath && diffPath[0] != '\0') {
      t.push_back({Misc::textureFromFile(diffPath), "texture_diffuse"});
    }
    if (specPath && specPath[0] != '\0') {
      t.push_back({Misc::textureFromFile(specPath), "texture_specular"});
    }

    mesh = new Mesh(Mesh::vertexFrom1D(v[typeNum], 4), {0, 1, 2, 0, 2, 3}, t);
  }
  void Draw(Shader &shader) override { mesh->Draw(shader); }
  ~Plane() { delete mesh; }

 private:
  Mesh *mesh;
};

#endif