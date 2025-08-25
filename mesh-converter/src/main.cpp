#include "assimp/vector3.h"
#include "mesh.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <meshoptimizer.h>
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

void processLODs(std::vector<uint32_t> indices,
                 const std::vector<float> &vertices,
                 std::vector<std::vector<uint32_t>> &outLODs) {
  size_t verticesCountIn = vertices.size() / 3;
  size_t targetIndicesCount = indices.size();
  uint8_t LOD = 1;
  std::print("\n   LOD0: {} indices", indices.size());
  outLODs.push_back(indices);
  while (targetIndicesCount > 1024 && LOD < 8) {
    targetIndicesCount = indices.size() / 2;
    bool sloppy = false;
    size_t numOptIndices = meshopt_simplify(
        indices.data(), indices.data(), (uint32_t)indices.size(),
        vertices.data(), verticesCountIn, sizeof(float) * 3, targetIndicesCount,
        0.02f);
    if (static_cast<size_t>(numOptIndices * 1.1f) > indices.size()) {
      if (LOD > 1) {
        numOptIndices = meshopt_simplifySloppy(
            indices.data(), indices.data(), (uint32_t)indices.size(),
            vertices.data(), verticesCountIn, sizeof(float) * 3,
            targetIndicesCount, 0.02f);
        sloppy = true;
        if (numOptIndices == indices.size())
          break;
      }
    } else
      break;
    indices.resize(numOptIndices);
    meshopt_optimizeVertexCache(indices.data(), indices.data(), indices.size(), verticesCountIn);
    std::print("\n   LOD{}: {} indices {}", LOD, numOptIndices, sloppy ? "[sloppy]" : "");
    LOD++;
    outLODs.push_back(indices);
  }
}

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
    data.vertexData.push_back(v.x);
    data.vertexData.push_back(v.y);
    data.vertexData.push_back(v.z);
    if (exportTextures) {
      data.vertexData.push_back(t.x);
      data.vertexData.push_back(t.y);
    }
    if (exportNormals) {
      data.vertexData.push_back(n.x);
      data.vertexData.push_back(n.y);
      data.vertexData.push_back(n.z);
    }
  }
  for (size_t i = 0; i < m->mNumFaces; i++) {
    const aiFace &f = m->mFaces[i];
    data.indexData.push_back(f.mIndices[0] + vertexOffset);
    data.indexData.push_back(f.mIndices[1] + vertexOffset);
    data.indexData.push_back(f.mIndices[2] + vertexOffset);
  }
  indexOffset += numIndices;
  vertexOffset += m->mNumVertices;
  return result;
}

bool loadFile(const char *fileName) {
  if (verbose)
    std::println("Loading {}...", fileName);
  const unsigned int flags =
      aiProcess_JoinIdenticalVertices | aiProcess_Triangulate |
      aiProcess_GenSmoothNormals | aiProcess_PreTransformVertices |
      aiProcess_RemoveRedundantMaterials | aiProcess_FindDegenerates |
      aiProcess_FindInvalidData | aiProcess_FindInstances |
      aiProcess_OptimizeMeshes;

  const aiScene *scene = aiImportFile(fileName, flags);
  if (!scene || !scene->HasMeshes()) {
    std::println("Unable to load {}", fileName);
    return false;
  }

  data.meshes.reserve(scene->mNumMeshes);
  for (size_t i = 0; i < scene->mNumMeshes; i++)
    data.meshes.push_back(convertAiMesh(scene->mMeshes[i]));
  return true;
}

inline void saveMeshedToFile(FILE *f) {
  const MeshFileHeader header = {
      .magicValue = 0x12345678,
      .meshCount = (uint32_t)data.meshes.size(),
      .dataBlockStartOffset = (uint32_t)(sizeof(MeshFileHeader) +
                                         data.meshes.size() * sizeof(Mesh)),
      .indexDataSize =
          static_cast<uint32_t>(data.indexData.size() * sizeof(uint32_t)),
      .vertexDataSize =
          static_cast<uint32_t>(data.vertexData.size() * sizeof(float))};
  fwrite(&header, 1, sizeof(header), f);
  fwrite(data.meshes.data(), header.meshCount, sizeof(Mesh), f);
  fwrite(data.indexData.data(), 1, header.indexDataSize, f);
  fwrite(data.vertexData.data(), 1, header.vertexDataSize, f);
}

int main(int argc, char **argv) {
  bool exportTextures = false;
  bool exportNormals = false;

  if (argc < 3) {
    std::println("Usage: meshconvert <input> <output> [--export-texcoords | "
                 "-t] [--export-normals | -n]");
    std::println("Options: ");
    std::println("\t--export-texcoords | -t: export texture coordinates");
    std::println("\t--export-normals | -n: export normals");
    exit(255);
  }

  for (int i = 3; i < argc; i++) {
    exportTextures |=
        !strcmp(argv[i], "--export-texcords") || !strcmp(argv[i], "-t");
    exportNormals |=
        !strcmp(argv[i], "--export-normals") || !strcmp(argv[i], "-n");
    const bool exportAll = !strcmp(argv[i], "-tn") || !strcmp(argv[i], "-nt");
    exportTextures |= exportAll;
    exportNormals |= exportAll;
  }
  if (exportTextures)
    numElementsToStore += 2;
  if (exportNormals)
    numElementsToStore += 3;
  if (!loadFile(argv[1]))
    exit(255);
  FILE *f = fopen(argv[2], "wb");
  saveMeshedToFile(f);
  fclose(f);
  return 0;
}
