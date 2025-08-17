#include "model.h"
#include "glad/glad.h"
#include "glm/common.hpp"
#include "glm/fwd.hpp"
#include "mesh.h"
#include "shader.h"
#include "stb_image.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/version.h>
#include <cstdlib>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <print>
#include <meshoptimizer.h>

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

void drawModel(const Model &model, GLuint shader, glm::mat4 v, glm::mat4 p) {
  auto m = glm::mat4(1.0f); 
  m = glm::translate(m, model.position);
  m = glm::rotate(m, glm::radians(model.rotationAngle), model.rotation);
  m = glm::scale(m, glm::vec3(model.scale));

  PerFrameData perFrameData = { .mvp = p * v * m, .isWireframe = false };
  use(shader);

  glBindVertexArray(model.vao);

  glNamedBufferSubData(model.perFrameDataBuffer, 0, kBufferSize, &perFrameData);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDrawArrays(GL_TRIANGLES, 0, model.numVerts);

  perFrameData.isWireframe = true;
  glNamedBufferSubData(model.perFrameDataBuffer, 0, kBufferSize, &perFrameData);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDrawArrays(GL_TRIANGLES, 0, model.numVerts);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void loadModel(Model &model, const char *path) {
  glCreateVertexArrays(1, &model.vao);
  glBindVertexArray(model.vao);

  glCreateBuffers(1, &model.perFrameDataBuffer);
  glNamedBufferStorage(model.perFrameDataBuffer, kBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
  glBindBufferRange(GL_UNIFORM_BUFFER, 0, model.perFrameDataBuffer, 0, kBufferSize);

  glEnable(GL_POLYGON_OFFSET_LINE);
  glPolygonOffset(-1.0f, -1.0f);

  glCreateBuffers(1, &model.meshData);

  const aiScene* scene = aiImportFile(path, aiProcess_Triangulate);

  if (!scene || !scene->HasMeshes()) {
    std::println("Unable to load file");
    exit(EXIT_FAILURE);
  }

  std::vector<glm::vec3> positions;
  std::vector<unsigned int> indices;
  const aiMesh* mesh = scene->mMeshes[0];
  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    const aiVector3D v = mesh->mVertices[i];
    positions.push_back(glm::vec3(v.x, v.z, v.y));
  }
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    for (unsigned int j = 0; j < 3; j++) {
      indices.push_back(mesh->mFaces[i].mIndices[j]);
    }
  }
  aiReleaseImport(scene);

  std::vector<unsigned int> remap(indices.size());
  const size_t vertexCount = meshopt_generateVertexRemap(remap.data(), indices.data(), indices.size(), positions.data(), indices.size(), sizeof(glm::vec3));

  std::vector<unsigned int> remappedIndices(indices.size());
  std::vector<glm::vec3> remappedVertices(vertexCount);

  meshopt_remapIndexBuffer(remappedIndices.data(), indices.data(), indices.size(), remap.data());
  meshopt_remapVertexBuffer(remappedVertices.data(), positions.data(), positions.size(), sizeof(glm::vec3), remap.data());

  

  glNamedBufferStorage(model.meshData, sizeof(glm::vec3) * positions.size(), positions.data(), 0);
  
  glVertexArrayVertexBuffer(model.vao, 0, model.meshData, 0, sizeof(glm::vec3));
  glEnableVertexArrayAttrib(model.vao, 0);
  glVertexArrayAttribFormat(model.vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribBinding(model.vao, 0, 0);

  model.numVerts = static_cast<int>(positions.size());
}
