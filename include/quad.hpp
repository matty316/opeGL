#pragma once

#include "vertex.hpp"
#include <glm/glm.hpp>
#include <vector>

class OpeQuad {
public:
  void addQuad(glm::vec3 position, float angle, glm::vec3 rotation, float scale,
               uint32_t textureIndex);
  uint32_t getInstanceCount();
  std::vector<PerInstanceData> &getPerInstanceData();

private:
  std::vector<PerInstanceData> instanceData;
};
