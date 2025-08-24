#pragma once

#include <cstdint>
#include <vector>
constexpr const uint32_t kMaxLODs = 8;
constexpr const uint32_t kMaxStreams = 8;

struct Mesh {
  uint32_t lodCount;
  uint32_t streamCount;
  uint32_t materialID;
  uint32_t meshSize;
  uint32_t vertexCount;
  uint32_t lodOffset[kMaxLODs];
  inline uint64_t lodSize(uint32_t lod) {
    return lodOffset[lod+1] - lodOffset[lod];
  }
  uint64_t streamOffset[kMaxStreams];
  uint32_t streamElementSize[kMaxStreams];
};

struct MeshFileHeader {
  uint32_t magicValue;
  uint32_t meshCount;
  uint32_t dataBlockStartOffset;
  uint32_t indexDataSize;
  uint32_t vertexDataSize;
};

struct MeshData {
  std::vector<uint32_t> indexData;
  std::vector<float> vertexData;
  std::vector<Mesh> meshes;
};
