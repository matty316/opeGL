#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

struct Cube {
  glm::vec3 pos, rotation;
  float angle, scale;
  GLuint vao, vbo, diff, spec;
};

Cube createCube(const char* diff, const char* spec, glm::vec3 pos, glm::vec3 rotation, float angle, float scale);
void drawCube(Cube& cube, GLuint shader);
