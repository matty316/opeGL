#pragma once

#include "cube.h"
#include <cstddef>
#include <glm/glm.hpp>

enum ChunkType { Sphere, Landscape, Wall, Roof };

struct Chunk {
  glm::vec3 pos, rotation;
  float angle, scale;
  size_t diff, spec;
  GLuint vao, vbo, uvBuffer;
  std::vector<GLfloat> vertices;
  size_t vertSize = 0, chunkSize = 64, height = 64, width = 64, depth = 64, xoffset = 0, zoffset = 0;
};

Chunk createChunk(size_t diff, size_t spec, glm::vec3 pos, glm::vec3 rotations,
                  float angle, float scale, ChunkType type,
                  bool shouldSetupBuffers = true, size_t xoffset = 0,
                  size_t zoffset = 0, size_t chunkSize = 64, size_t height = 0,
                  size_t width = 0, size_t depth = 0);
void drawChunk(Chunk &chunk, GLuint shader, glm::mat4 vp);

struct Terrain {
  glm::vec3 pos, rotation;
  float angle, scale;
  GLuint vao, vbo;
  std::vector<Chunk> chunks;
  std::vector<GLfloat> vertices;
  size_t width = 16, depth = 16, vertSize = 0;
};

Terrain createTerrain(size_t width, size_t depth);
void drawTerrain(Terrain terrain, GLuint shader);
