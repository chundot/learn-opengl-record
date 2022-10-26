#ifndef MODEL_HPP
#define MODEL_HPP
#include <cstring>
#include <string>
#include <vector>
#include "shader.hpp"
#include "mesh.hpp"
#include "misc.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
class Model {
 public:
  Model(const char *path) { loadModel(path); }
  void Draw(Shader shader) {
    shader.use();
    for (int i = 0; i < meshes.size(); ++i) {
      meshes[i].Draw(shader);
    }
  }

 private:
  std::vector<Mesh> meshes;
  std::string directory;
  std::vector<Texture> texturesLoaded;
  void loadModel(const std::string path) {
    Assimp::Importer importer;
    auto scene =
        importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) {
      std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
      return;
    }
    directory = path.substr(0, path.find_last_of('\\'));
    processNode(scene->mRootNode, scene);
  }
  void processNode(aiNode *node, const aiScene *scene) {
    // 处理节点的网格
    for (GLuint i = 0; i < node->mNumMeshes; ++i) {
      auto mesh = scene->mMeshes[node->mMeshes[i]];
      meshes.push_back(processMesh(mesh, scene));
    }
    // 递归处理子节点
    for (GLuint i = 0; i < node->mNumChildren; ++i) {
      processNode(node->mChildren[i], scene);
    }
  }
  Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    for (GLuint i = 0; i < mesh->mNumVertices; ++i) {
      Vertex vertex;
      // 顶点位置处理
      glm::vec3 vector;
      vector.x = mesh->mVertices[i].x, vector.y = mesh->mVertices[i].y,
      vector.z = mesh->mVertices[i].z;
      vertex.Position = vector;
      // 法线
      vector.x = mesh->mNormals[i].x, vector.y = mesh->mNormals[i].y,
      vector.z = mesh->mNormals[i].z;
      vertex.Normal = vector;
      // 纹理坐标
      if (mesh->mTextureCoords[0]) {
        glm::vec2 vec;
        vec.x = mesh->mTextureCoords[0][i].x,
        vec.y = mesh->mTextureCoords[0][i].y;
        vertex.TexCoords = vec;
      } else
        vertex.TexCoords = glm::vec2(0, 0);

      // 存储顶点
      vertices.push_back(vertex);
    }
    // 处理索引
    for (GLuint i = 0; i < mesh->mNumFaces; ++i) {
      auto face = mesh->mFaces[i];
      for (GLuint j = 0; j < face.mNumIndices; ++j)
        indices.push_back(face.mIndices[j]);
    }
    // 处理材质
    if (mesh->mMaterialIndex >= 0) {
      auto material = scene->mMaterials[mesh->mMaterialIndex];
      auto diffMaps = loadMaterialTexture(material, aiTextureType_DIFFUSE,
                                          "texture_diffuse");
      textures.insert(textures.end(), diffMaps.begin(), diffMaps.end());
      auto specMaps = loadMaterialTexture(material, aiTextureType_SPECULAR,
                                          "texture_specular");
      textures.insert(textures.end(), specMaps.begin(), specMaps.end());
    }
    return Mesh(vertices, indices, textures);
  }
  std::vector<Texture> loadMaterialTexture(aiMaterial *mat, aiTextureType type,
                                           std::string typeName) {
    std::vector<Texture> textures;
    for (GLuint i = 0; i < mat->GetTextureCount(type); ++i) {
      bool skip = false;
      aiString str;
      mat->GetTexture(type, i, &str);
      for (int j = 0; j < texturesLoaded.size(); ++j)
        if (std::strcmp(texturesLoaded[j].path.data, str.C_Str()) == 0) {
          textures.push_back(texturesLoaded[j]), skip = true;
          break;
        }
      if (!skip) {
        Texture texture;
        texture.id = Misc::textureFromFile(str.C_Str(), directory);
        texture.type = typeName;
        texture.path = str;
        textures.push_back(texture);
        texturesLoaded.push_back(texture);  // 添加到已加载的纹理中
      }
    }
    return textures;
  }
};

#endif