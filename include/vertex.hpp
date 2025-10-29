#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Vertex {
  glm::vec3 pos;
  glm::vec2 texCoord;
  glm::vec3 normal = {0.0f, 0.0f, 1.0f};
};

struct PerInstanceData {
  glm::mat4 model;
  uint32_t textureIndex;
  glm::vec3 padding = glm::vec3(0.0f);
};

const std::vector<Vertex> quadVertices = {{{-0.5f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                                          {{0.5f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                                          {{0.5f, 1.0f, 0.0f}, {0.0f, 1.0f}},
                                          {{-0.5f, 1.0f, 0.0f}, {1.0f, 1.0f}}};

const std::vector<uint32_t> quadIndices = {0, 1, 2, 2, 3, 0};
