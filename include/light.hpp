#pragma once

#include <glm/glm.hpp>

struct PointLight {
  PointLight(glm::vec3 position) : position(position) {}
  alignas(16) glm::vec3 position{0.0f};
  alignas(16) glm::vec3 ambient{0.2f};
  alignas(16) glm::vec3 diffuse{0.5f};
  alignas(16) glm::vec3 specular{1.0f};
  float constant = 1.0f;
  float linear = 0.09f;
  float quadratic = 0.032f;
};
