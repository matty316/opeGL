#include "quad.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

void OpeQuad::addQuad(glm::vec3 position, float angle, glm::vec3 rotation,
                      float scale) {
  auto model = glm::mat4(1.0f);
  model = glm::translate(model, position);
  auto rot = glm::angleAxis(glm::radians(angle), rotation);
  model = model * glm::mat4_cast(rot);
  model = glm::scale(model, glm::vec3(scale));
  matrices.push_back(model);
}

uint32_t OpeQuad::getInstanceCount() {
  return static_cast<uint32_t>(matrices.size());
}

std::vector<glm::mat4> &OpeQuad::getMatrices() { return matrices; }
