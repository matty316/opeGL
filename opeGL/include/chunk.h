#pragma once

#include "cube.h"
#include <glm/glm.hpp>

#define CHUNK_SIZE 16

struct Chunk {
  int id;
  Cube cubes[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
};

void loadChunk(int id);
void unloadChunk(int id);
