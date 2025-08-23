#include <cstdint>
#include <vector>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include "mesh.h"

bool verbose = true;

MeshData data;

uint32_t indexOffset = 0;
uint32_t vertextOffset = 0;
bool exportTextures = false;
bool exportNormals = false;

uint32_t numElementsToStore = 3;

Mesh convertAiMesh(const aiMesh* m) {
  const bool hasTexCoords = m->HasTextureCoords(0);
  const uint32_t numIndices = m->mNumFaces * 3;
  const uint32_t numElements = numElementsToStore;
  const uint32_t streamElementSize = static_cast<uint32_t>(numElements * sizeof(float));
  const uint32_t meshSize = static_cast<uint32_t>(m->mNumVertices * streamElementSize * sizeof(uint32_t)); 
  const Mesh result = {
  .lodCount = 1,
  .streamCount = 1,
  .materialID = 0,
  .meshSize = meshSize,
  .vertexCount = m->mNumVertices,
  .lodOffset = { indexOffset * sizeof(uint32_t), (indexOffset + numIndices) * sizeof(uint32_t) },
  .streamOffset = { vertextOffset * streamElementSize },
  .streamElementSize = { streamElementSize },
};
}
