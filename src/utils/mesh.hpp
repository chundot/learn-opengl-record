#ifndef MESH_HPP
#define MESH_HPP
#include <string>
#include <vector>
#include "glm.hpp"
#include "shader.hpp"
#include <assimp/Importer.hpp>
struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
};
struct Texture {
  unsigned int id;
  std::string type;
  aiString path;
};
class Mesh {
 public:
  /*  网格数据  */
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;
  /*  函数  */
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
       std::vector<Texture> textures)
      : vertices(vertices), indices(indices), textures(textures) {
    setupMesh();
  }
  void Draw(Shader &shader) {
    if (shader.hasTexture) {
      unsigned int diffNum = 1, specNum = 1;
      for (int i = 0; i < textures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string num, name = textures[i].type;
        if (name == "texture_diffuse")
          num = std::to_string(diffNum++);
        else if (name == "texture_specular")
          num = std::to_string(specNum++);
        shader.setU(("material." + name + num).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
      }
      glActiveTexture(GL_TEXTURE0);
    }
    // 绘制网格
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
  static std::vector<Vertex> vertexFrom1D(std::vector<float> arr,
                                          int numOfVertex) {
    std::vector<Vertex> v;
    static glm::vec3 defNormal = {0, 1, 0};
    int vertexSz = (int)arr.size() / numOfVertex;
    for (int i = 0; i < numOfVertex; ++i) {
      int idx = i * vertexSz, offset = 3;
      glm::vec3 normal = defNormal;
      if (vertexSz > 5) {
        normal = {arr[idx + offset], arr[idx + offset + 1],
                  arr[idx + offset + 2]};
        offset += 3;
      }
      Vertex vertex{{arr[idx], arr[idx + 1], arr[idx + 2]},
                    normal,
                    {arr[idx + offset], arr[idx + offset + 1]}};
      v.push_back(vertex);
    }
    return v;
  }

 private:
  /*  渲染数据  */
  unsigned int VAO, VBO, EBO;
  /*  函数  */
  void setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);

    // 顶点位置
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    // 顶点法线
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, Normal));
    // 顶点纹理坐标
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, TexCoords));
    glBindVertexArray(0);
  }
};
#endif