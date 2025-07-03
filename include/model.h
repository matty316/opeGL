#pragma once

#include "mesh.h"
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class Model {
public:
  glm::vec3 position{0.0f};
  glm::vec3 rotation{1.0f};
  float rotationAngle = 0.0f;
  float scale = 0.5f;

  Model(std::string path, glm::vec3 pos, glm::vec3 rotation,
        float rotationAngle, float scale);
  void draw(Shader shader);

private:
  std::vector<Mesh> meshes;
  std::string dir;
  std::vector<Texture> textures_loaded;

  void loadModel(std::string path);
  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);
  std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                            std::string typeName);
  unsigned int TextureFromFile(const char *path, const std::string &directory);
};
