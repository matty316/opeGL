#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

struct Plane {
  GLuint vao, vbo, ebo, diffuse, specular;
  glm::vec3 pos, rotation;
  float angle, scale;
};

Plane createPlane(const char *diffusePath, const char *specularPath, glm::vec3 pos, glm::vec3 rotation, float angle, float scale);
void drawPlane(Plane plane, GLuint shader);
