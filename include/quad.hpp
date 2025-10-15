#pragma once

#include <glm/glm.hpp>
#include <vector>

class OpeQuad {
public:
  void addQuad(glm::vec3 position, float angle, glm::vec3 rotation,
               float scale);
  uint32_t getInstanceCount();
  std::vector<glm::mat4> &getMatrices();

private:
  std::vector<glm::mat4> matrices;
};
