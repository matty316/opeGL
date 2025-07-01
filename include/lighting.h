#pragma once

#include <glm/glm.hpp>

class DirectionalLight {
public:
  DirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse,
                   glm::vec3 specular)
      : direction(direction), ambient(ambient), diffuse(diffuse),
        specular(specular) {}

  glm::vec3 direction;
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
};

class PointLight {
public:
  PointLight(glm::vec3 pos) : pos(pos) {}
  PointLight(glm::vec3 pos, glm::vec3 ambient, glm::vec3 diffuse,
             glm::vec3 specular)
      : pos(pos), ambient(ambient), diffuse(diffuse), specular(specular) {}

  glm::vec3 pos;
  glm::vec3 ambient{0.05f};
  glm::vec3 diffuse{0.8f};
  glm::vec3 specular{1.0f};
  float constant = 1.0f;
  float linear = 0.09f;
  float quadratic = 0.032f;
};
