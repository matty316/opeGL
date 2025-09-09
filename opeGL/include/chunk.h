#pragma once

#include "cube.h"
#include <glm/glm.hpp>

enum ChunkType {
  Sphere, Landscape, Wall, Roof
};

struct Chunk {
  glm::vec3 pos, rotation;
  float angle, scale;
  size_t diff, spec;
  GLuint vao, vbo, uvBuffer;
  std::vector<GLfloat> vertices;
  size_t vertSize = 0, chunkSize = 64;
};

Chunk createChunk(size_t diff, size_t spec, glm::vec3 pos, glm::vec3 rotations, float angle, float scale, ChunkType type, size_t chunkSize);
void drawChunk(Chunk &chunk, GLuint shader);
