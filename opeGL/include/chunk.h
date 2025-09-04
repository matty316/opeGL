#pragma once

#include "cube.h"
#include <glm/glm.hpp>

#define CHUNK_SIZE 32

struct Chunk {
  glm::vec3 pos;
  float scale;
  GLuint vao, vbo, diff, spec;
  std::vector<GLfloat> vertices;
  size_t vertSize;
  Cube cubes[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
};

Chunk createChunk(glm::vec3 pos, float scale);
void drawChunk(Chunk &chunk, GLuint shader);
