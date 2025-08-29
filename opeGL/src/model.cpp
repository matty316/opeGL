#include "model.h"
#include "glm/ext/matrix_transform.hpp"
#include "shader.h"
#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cstddef>
#include <cstdint>
#include <glm/gtc/type_ptr.hpp>
#include <meshoptimizer.h>
#include <print>
#include <stb_image.h>
#include <string>
#include <vector>

struct PerFrameData {
  glm::mat4 mvp;
  int isWireframe;
};
const GLsizeiptr kBufferSize = sizeof(PerFrameData);

void loadModel(Model &model, const char *path);
void processNode(Model &model, aiNode *node, const aiScene *scene);
Mesh processMesh(Model &model, aiMesh *mesh, const aiScene *scene);
std::vector<Texture> loadMaterialTextures(Model &model, aiMaterial *mat,
                                          aiTextureType type,
                                          std::string typeName);
Mesh createMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
                std::vector<Texture> textures);

void drawMesh(Mesh &mesh, GLuint Shader);

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
   const unsigned int flags =
      aiProcess_JoinIdenticalVertices | aiProcess_Triangulate |
      aiProcess_GenSmoothNormals | aiProcess_PreTransformVertices |
      aiProcess_RemoveRedundantMaterials | aiProcess_FindDegenerates |
      aiProcess_FindInvalidData | aiProcess_FindInstances |
      aiProcess_OptimizeMeshes;
  const aiScene *scene =
      import.ReadFile(path, flags);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::println("ERROR::ASSIMP:: {} ", import.GetErrorString());
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
    std::println("Texture failed to load at path: {}", path);
    stbi_image_free(data);
  }

  return textureID;
}

Mesh createMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
                std::vector<Texture> textures) {
  Mesh mesh;

  mesh.vertices = vertices;
  mesh.indices = indices;
  mesh.textures = textures;

  glGenVertexArrays(1, &mesh.vao);
  glGenBuffers(1, &mesh.vbo);
  glGenBuffers(1, &mesh.ebo);

  glBindVertexArray(mesh.vao);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

  glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex),
               &mesh.vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t),
               &mesh.indices[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, normal));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, texCoords));

  glBindVertexArray(0);
  return mesh;
}

void drawMesh(Mesh &mesh, GLuint shader) {
  uint32_t diffuseNr = 1;
  uint32_t specularNr = 1;
  for (uint32_t i = 0; i < mesh.textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);

    std::string number;
    std::string name = mesh.textures[i].type;
    if (name == "texture_diffuse")
      number = std::to_string(diffuseNr++);
    else if (name == "texture_specular")
      number = std::to_string(specularNr++);

    setInt(shader, "material." + name + number, i);
    glBindTexture(GL_TEXTURE_2D, mesh.textures[i].id);
  }
  glActiveTexture(GL_TEXTURE0);

  glBindVertexArray(mesh.vao);
  glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
