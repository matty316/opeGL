#include "model.hpp"

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure

#include <glm/gtc/quaternion.hpp>

#include <stdexcept>

void OpeModel::loadModel() {
  Assimp::Importer importer;

  const aiScene *scene = importer.ReadFile(
      modelPath.c_str(),
      aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
          aiProcess_FlipUVs | aiProcess_GenSmoothNormals |
          aiProcess_SplitLargeMeshes | aiProcess_ImproveCacheLocality |
          aiProcess_RemoveRedundantMaterials | aiProcess_FindDegenerates |
          aiProcess_FindInvalidData | aiProcess_GenUVCoords |
          aiProcess_CalcTangentSpace);

  if (scene == nullptr || !scene->HasMeshes())
    throw std::runtime_error("unable to load model");

  for (size_t i = 0; i < scene->mNumMeshes; i++) {
    auto mesh = scene->mMeshes[i];
    for (size_t j = 0; j < mesh->mNumVertices; j++) {
      Vertex vertex;
      vertex.pos = {mesh->mVertices[j].x, mesh->mVertices[j].y,
                    mesh->mVertices[j].z};
      vertex.texCoord = {mesh->mTextureCoords[0][j].x,
                         mesh->mTextureCoords[0][j].y};
      vertex.normal = {mesh->mNormals[j].x, mesh->mNormals[i].y,
                       mesh->mNormals[j].z};
      vertices.push_back(vertex);
    }
    for (size_t j = 0; j < mesh->mNumFaces; j++) {
      aiFace face = mesh->mFaces[j];
      for (size_t k = 0; k < face.mNumIndices; k++) {
        indices.push_back(face.mIndices[k]);
      }
    }
    auto *material = scene->mMaterials[mesh->mMaterialIndex];
    aiString str;
    material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
    auto directory = modelPath.substr(0, modelPath.find_last_of('/'));
    diffusePath = directory + "/" + str.C_Str();
  }

  glCreateBuffers(1, &vbo);
  glNamedBufferStorage(vbo, sizeof(Vertex) * vertices.size(), vertices.data(),
                       GL_DYNAMIC_STORAGE_BIT);

  glCreateBuffers(1, &ebo);
  glNamedBufferStorage(ebo, sizeof(uint32_t) * indices.size(), indices.data(),
                       GL_DYNAMIC_STORAGE_BIT);

  glCreateVertexArrays(1, &vao);

  glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));
  glVertexArrayElementBuffer(vao, ebo);

  glEnableVertexArrayAttrib(vao, 0);
  glEnableVertexArrayAttrib(vao, 1);
  glEnableVertexArrayAttrib(vao, 2);

  glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE,
                            offsetof(Vertex, pos));
  glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE,
                            offsetof(Vertex, texCoord));
  glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, GL_FALSE,
                            offsetof(Vertex, normal));

  glVertexArrayAttribBinding(vao, 0, 0);
  glVertexArrayAttribBinding(vao, 1, 0);
  glVertexArrayAttribBinding(vao, 2, 0);
}

glm::mat4 OpeModel::modelMatrix() {
  auto model = glm::mat4(1.0f);
  model = glm::translate(model, pos);
  glm::quat rot = glm::angleAxis(glm::radians(angle), rotation);
  model = model * glm::mat4_cast(rot);
  model = glm::scale(model, glm::vec3(scale));
  return model;
}

uint32_t OpeModel::getIndicesSize() {
  return static_cast<uint32_t>(indices.size());
}

void OpeModel::draw(uint32_t offset) {
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
