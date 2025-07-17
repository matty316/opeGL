#include "model.h"
#include "glad/glad.h"
#include "glm/common.hpp"
#include "mesh.h"
#include "stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <iostream>

void loadModel(Model &model, const char *path);
void processNode(Model &model, aiNode *node, const aiScene *scene);
Mesh processMesh(Model &model, aiMesh *mesh, const aiScene *scene);
std::vector<Texture> loadMaterialTextures(Model &model, aiMaterial *mat,
                                          aiTextureType type,
                                          std::string typeName);
unsigned int TextureFromFile(const char *path, const std::string &directory);

Model createModel(const char *path, glm::vec3 pos, glm::vec3 rotation,
                  float rotationAngle, float scale) {
  Model model;
  model.position = pos;
  model.rotation = rotation;
  model.rotationAngle = rotationAngle;
  model.scale = scale;
  loadModel(model, path);
  return model;
}

void drawModel(Model &model, GLuint shader) {
  auto modelMatrix = glm::mat4{1.0f};
  modelMatrix = glm::translate(modelMatrix, model.position);
  modelMatrix = glm::rotate(modelMatrix, glm::radians(model.rotationAngle),
                            model.rotation);
  modelMatrix = glm::scale(modelMatrix, glm::vec3{model.scale});
  setMat4(shader, "model", modelMatrix);

  for (auto &mesh : model.meshes) {
    drawMesh(mesh, shader);
  }
}

void loadModel(Model &model, const char *path) {
  Assimp::Importer import;
  const aiScene *scene =
      import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << "\n";
    return;
  }

  std::string pathStr{path};
  model.dir = pathStr.substr(0, pathStr.find_last_of('/'));
  processNode(model, scene->mRootNode, scene);
}

void processNode(Model &model, aiNode *node, const aiScene *scene) {
  for (size_t i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    model.meshes.push_back(processMesh(model, mesh, scene));
  }
  for (size_t i = 0; i < node->mNumChildren; i++) {
    processNode(model, node->mChildren[i], scene);
  }
}

Mesh processMesh(Model &model, aiMesh *mesh, const aiScene *scene) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  for (size_t i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    glm::vec3 vector;
    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.position = vector;
    vector.x = mesh->mNormals[i].x;
    vector.y = mesh->mNormals[i].y;
    vector.z = mesh->mNormals[i].z;
    vertex.normal = vector;
    if (mesh->mTextureCoords[0]) {
      glm::vec2 vec;
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.texCoords = vec;
    } else {
      vertex.texCoords = glm::vec2(0.0f);
    }
    vertices.push_back(vertex);
  }

  for (size_t i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (size_t j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
  std::vector<Texture> diffuseMaps = loadMaterialTextures(
      model, material, aiTextureType_DIFFUSE, "texture_diffuse");
  textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
  std::vector<Texture> specularMaps = loadMaterialTextures(
      model, material, aiTextureType_SPECULAR, "texture_specular");
  textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

  return createMesh(vertices, indices, textures);
}

std::vector<Texture> loadMaterialTextures(Model &model, aiMaterial *mat,
                                          aiTextureType type,
                                          std::string typeName) {
  std::vector<Texture> textures;
  for (size_t i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    bool skip = false;

    for (size_t j = 0; j < model.textures_loaded.size(); j++) {
      if (std::strcmp(model.textures_loaded[j].path.data(), str.C_Str()) == 0) {
        textures.push_back(model.textures_loaded[j]);
        skip = true;
        break;
      }
    }
    if (!skip) {
      Texture texture;
      texture.id = TextureFromFile(str.C_Str(), model.dir);
      texture.type = typeName;
      texture.path = str.C_Str();
      textures.push_back(texture);
      model.textures_loaded.push_back(texture);
    }
  }
  return textures;
}

unsigned int TextureFromFile(const char *path, const std::string &directory) {
  std::string filename = std::string(path);
  filename = directory + '/' + filename;

  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data =
      stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}
