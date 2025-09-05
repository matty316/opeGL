#pragma once

#include "cube.h"
#include <glm/glm.hpp>

#define CHUNK_SIZE 64

struct Chunk {
  glm::vec3 pos, rotation;
  float angle, scale;
  GLuint vao, vbo, diff, spec;
  std::vector<GLfloat> vertices;
  size_t vertSize = 0;
  Cube *cubes;
};

Chunk createChunk(glm::vec3 pos, glm::vec3 rotations, float angle, float scale);
void drawChunk(Chunk &chunk, GLuint shader);
