#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/fwd.hpp>

struct Cube {
  glm::vec3 pos, rotation;
  float angle, scale;
  GLuint vao, vbo, diff, spec;
  bool isActive = true, top = true, bottom = true, front = true, back = true, left = true, right = true;
};

Cube createCube(GLuint diff, GLuint spec, glm::vec3 pos, glm::vec3 rotation, float angle, float scale);
void drawCube(Cube &cube, GLuint shader);
