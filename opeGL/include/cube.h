#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

struct Cube {
  glm::vec3 pos, rotation;
  float angle, scale;
};

Cube createCube(const char* diffPath, const char* specPath, glm::vec3 pos, glm::vec3 rotation, float angle, float scale);
void drawCube(Cube& cube, GLuint shader);
