#pragma once

#include "cube.h"
#include <cstddef>
#include <glm/glm.hpp>

enum ChunkType { Sphere, Landscape, Wall, Roof };

struct Chunk {
  glm::vec3 pos = glm::vec3(0.0f), rotation = glm::vec3(1.0f);
  float angle = 0.0f, scale = 1.0f;
  size_t diff = 0, spec = 0;
  GLuint vao = 0, vbo = 0, uvBuffer = 0;
  std::vector<GLuint> vertices;
  size_t vertSize = 0, chunkSize = 64, height = 64, width = 64, depth = 64,
         xoffset = 0, zoffset = 0;
};

Chunk createChunk(size_t diff, size_t spec, glm::vec3 pos, glm::vec3 rotations,
                  float angle, float scale, ChunkType type,
                  bool shouldSetupBuffers = true, size_t chunkSize = 64,
                  size_t height = 0, size_t width = 0, size_t depth = 0);
void drawChunk(Chunk &chunk, GLuint shader, glm::mat4 vp);

struct Terrain {
  glm::vec3 pos, rotation;
  float angle, scale;
  GLuint vao, vbo;
  size_t width = 16, depth = 16, vertSize = 0;
};

Terrain createTerrain(size_t width, size_t depth, float scale = 1.0f);
void drawTerrain(Terrain &terrain, GLuint shader, glm::mat4 vp);
void updateTerrain(Terrain &terrain, glm::vec3 pos);
void subTerrainData(Terrain &terrain);
