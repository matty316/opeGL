#include "quad.hpp"
#include "vertex.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

void OpeQuad::addQuad(glm::vec3 position, float angle, glm::vec3 rotation,
                      float scale, uint32_t textureIndex) {
  auto model = glm::mat4(1.0f);
  model = glm::translate(model, position);
  auto rot = glm::angleAxis(glm::radians(angle), rotation);
  model = model * glm::mat4_cast(rot);
  model = glm::scale(model, glm::vec3(scale));

  PerInstanceData data{.model = model, .textureIndex = textureIndex};

  instanceData.push_back(data);
}

uint32_t OpeQuad::getInstanceCount() {
  return static_cast<uint32_t>(instanceData.size());
}

std::vector<PerInstanceData> &OpeQuad::getPerInstanceData() {
  return instanceData;
}
