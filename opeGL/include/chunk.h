#pragma once

#include "cube.h"
#include <glm/glm.hpp>

#define CHUNK_SIZE 16

struct Chunk {
  glm::vec3 pos;
  Cube cubes[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
};

Chunk createChunk(glm::vec3 pos);
void drawChunk(Chunk &chunk, GLuint shader);
