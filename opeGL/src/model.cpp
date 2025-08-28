#include "model.h"
#include "glm/ext/matrix_transform.hpp"
#include "shader.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/gtc/type_ptr.hpp>
#include <meshoptimizer.h>
#include <print>
#include <vector>

struct PerFrameData {
  glm::mat4 mvp;
  int isWireframe;
};
const GLsizeiptr kBufferSize = sizeof(PerFrameData);

void loadModel(Model &model, const char *path);

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

void drawModel(const Model &model, GLuint shader, glm::mat4 v, glm::mat4 p) {
  auto m = glm::mat4(1.0f);
  m = glm::translate(m, model.position);
  m = glm::scale(m, glm::vec3(model.scale));

  PerFrameData perFrameData = {.mvp = p * v * m, .isWireframe = false};
  use(shader);

  glBindVertexArray(model.vao);

  glNamedBufferSubData(model.perFrameDataBuffer, 0, kBufferSize, &perFrameData);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDrawElements(GL_TRIANGLES, model.indicesSize, GL_UNSIGNED_INT, 0);

  perFrameData.isWireframe = true;
  glNamedBufferSubData(model.perFrameDataBuffer, 0, kBufferSize, &perFrameData);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDrawElements(GL_TRIANGLES, model.indicesSize, GL_UNSIGNED_INT, 0);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void loadModel(Model &model, const char *path) {
  glCreateVertexArrays(1, &model.vao);
  glBindVertexArray(model.vao);

  glCreateBuffers(1, &model.perFrameDataBuffer);
  glNamedBufferStorage(model.perFrameDataBuffer, kBufferSize, nullptr,
                       GL_DYNAMIC_STORAGE_BIT);
  glBindBufferRange(GL_UNIFORM_BUFFER, 0, model.perFrameDataBuffer, 0,
                    kBufferSize);

  glEnable(GL_POLYGON_OFFSET_LINE);
  glPolygonOffset(-1.0f, -1.0f);

  glCreateBuffers(1, &model.meshData);
  const unsigned int flags =
      aiProcess_JoinIdenticalVertices | aiProcess_Triangulate |
      aiProcess_GenSmoothNormals | aiProcess_PreTransformVertices |
      aiProcess_RemoveRedundantMaterials | aiProcess_FindDegenerates |
      aiProcess_FindInvalidData | aiProcess_FindInstances |
      aiProcess_OptimizeMeshes;

  const aiScene *scene = aiImportFile(path, flags);

  if (!scene || !scene->HasMeshes()) {
    std::println("Unable to load file");
    exit(EXIT_FAILURE);
  }

  std::vector<glm::vec3> positions;
  std::vector<unsigned int> indices;
  const aiMesh *mesh = scene->mMeshes[0];
  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    const aiVector3D v = mesh->mVertices[i];
    positions.push_back(glm::vec3(v.x, v.y, v.z));
  }
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    for (unsigned int j = 0; j < 3; j++) {
      indices.push_back(mesh->mFaces[i].mIndices[j]);
    }
  }
  aiReleaseImport(scene);

  std::vector<unsigned int> remap(indices.size());
  const size_t vertexCount = meshopt_generateVertexRemap(
      remap.data(), indices.data(), indices.size(), positions.data(),
      indices.size(), sizeof(glm::vec3));

  std::vector<unsigned int> remappedIndices(indices.size());
  std::vector<glm::vec3> remappedVertices(vertexCount);

  meshopt_remapIndexBuffer(remappedIndices.data(), indices.data(),
                           indices.size(), remap.data());
  meshopt_remapVertexBuffer(remappedVertices.data(), positions.data(),
                            positions.size(), sizeof(glm::vec3), remap.data());

  meshopt_optimizeVertexCache(remappedIndices.data(), remappedIndices.data(),
                              indices.size(), vertexCount);
  meshopt_optimizeOverdraw(remappedIndices.data(), remappedIndices.data(),
                           indices.size(), glm::value_ptr(remappedVertices[0]),
                           vertexCount, sizeof(glm::vec3), 1.05f);
  meshopt_optimizeVertexFetch(remappedVertices.data(), remappedIndices.data(),
                              indices.size(), remappedVertices.data(),
                              vertexCount, sizeof(glm::vec3));

  const float threshold = 0.2f;
  const size_t target_index_count = size_t(remappedIndices.size() * threshold);
  const float target_error = 1e-2f;
  std::vector<unsigned int> indicesLod(remappedIndices.size());
  indicesLod.resize(meshopt_simplify(
      &indicesLod[0], remappedIndices.data(), remappedIndices.size(),
      &remappedVertices[0].x, vertexCount, sizeof(glm::vec3),
      target_index_count, target_error));

  indices = remappedIndices;
  positions = remappedVertices;

  auto sizeIndicesLod = sizeof(unsigned int) * indicesLod.size();
  auto sizeVertices = sizeof(glm::vec3) * positions.size();

  glNamedBufferStorage(model.meshData, sizeIndicesLod + sizeVertices, nullptr,
                       GL_DYNAMIC_STORAGE_BIT);
  glNamedBufferSubData(model.meshData, 0, sizeIndicesLod, indicesLod.data());
  glNamedBufferSubData(model.meshData, sizeIndicesLod, sizeVertices,
                       positions.data());

  glVertexArrayElementBuffer(model.vao, model.meshData);
  glVertexArrayVertexBuffer(model.vao, 0, model.meshData, sizeIndicesLod,
                            sizeof(glm::vec3));
  glEnableVertexArrayAttrib(model.vao, 0);
  glVertexArrayAttribFormat(model.vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribBinding(model.vao, 0, 0);
  model.indicesSize = indicesLod.size();
}

Mesh createMesh() {
  
}
