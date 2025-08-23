#include "assimp/vector3.h"
#include "mesh.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cstdint>
#include <print>
#include <sys/types.h>
#include <vector>

bool verbose = true;

MeshData data;

uint32_t indexOffset = 0;
uint32_t vertexOffset = 0;
bool exportTextures = false;
bool exportNormals = false;

uint32_t numElementsToStore = 3;

Mesh convertAiMesh(const aiMesh *m) {
  const bool hasTexCoords = m->HasTextureCoords(0);
  const uint32_t numIndices = m->mNumFaces * 3;
  const uint32_t numElements = numElementsToStore;
  const uint32_t streamElementSize =
      static_cast<uint32_t>(numElements * sizeof(float));
  const uint32_t meshSize = static_cast<uint32_t>(
      m->mNumVertices * streamElementSize * sizeof(uint32_t));
  const Mesh result = {
      .lodCount = 1,
      .streamCount = 1,
      .materialID = 0,
      .meshSize = meshSize,
      .vertexCount = m->mNumVertices,
      .lodOffset = {static_cast<uint32_t>(indexOffset * sizeof(uint32_t)),
                    static_cast<uint32_t>((indexOffset + numIndices) *
                                          sizeof(uint32_t))},
      .streamOffset = {vertexOffset * streamElementSize},
      .streamElementSize = {streamElementSize},
  };
  for (size_t i = 0; i < m->mNumVertices; i++) {
    const aiVector3D &v = m->mVertices[i];
    const aiVector3D &n = m->mNormals[i];
    const aiVector3D &t = hasTexCoords ? m->mTextureCoords[0][i] : aiVector3D();
    data.vertextData.push_back(v.x);
    data.vertextData.push_back(v.y);
    data.vertextData.push_back(v.z);
    if (exportTextures) {
      data.vertextData.push_back(t.x);
      data.vertextData.push_back(t.y);
    }
    if (exportNormals) {
      data.vertextData.push_back(n.x);
      data.vertextData.push_back(n.y);
      data.vertextData.push_back(n.z);
    }
  }
  for (size_t i = 0; i < m->mNumFaces; i++) {
    const aiFace& f = m->mFaces[i];
    data.indexData.push_back(f.mIndices[0] + vertexOffset);
    data.indexData.push_back(f.mIndices[1] + vertexOffset);
    data.indexData.push_back(f.mIndices[2] + vertexOffset);
  }
  indexOffset += numIndices;
  vertexOffset += m->mNumVertices;
  return result;
}

bool loadFile(const char* fileName) {
  if (verbose) std::println("Loading {}...", fileName);
  const unsigned int flags = aiProcess_JoinIdenticalVertices
    | aiProcess_Triangulate 
    | aiProcess_GenSmoothNormals
    | aiProcess_PreTransformVertices
    | aiProcess_RemoveRedundantMaterials
    | aiProcess_FindDegenerates
    | aiProcess_FindInvalidData
    | aiProcess_FindInstances
    | aiProcess_OptimizeMeshes;


}
