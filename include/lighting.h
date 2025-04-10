#pragma once

#include <glm/glm.hpp>

struct DirectionalLight {
  DirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse,
                   glm::vec3 specular)
      : direction(direction), ambient(ambient), diffuse(diffuse),
        specular(specular) {}

  glm::vec3 direction;
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
};

struct PointLight {
  PointLight(glm::vec3 pos, glm::vec3 ambient, glm::vec3 diffuse,
             glm::vec3 specular)
      : pos(pos), ambient(ambient), diffuse(diffuse), specular(specular) {}

  glm::vec3 pos;
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float constant = 1.0f;
  float linear = 0.09f;
  float quadratic = 0.032f;
};
