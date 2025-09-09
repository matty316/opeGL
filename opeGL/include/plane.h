#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

struct Plane {
  GLuint vao, vbo, ebo;
  GLuint64 diff;
  glm::vec3 pos, rotation;
  float angle, scale;
  int tiling;
};

Plane createPlane(GLuint64 diff, glm::vec3 pos, glm::vec3 rotation, float angle,
                  float scale, int tiling);
void drawPlane(Plane plane, GLuint shader);
