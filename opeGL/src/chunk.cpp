#include "chunk.h"
#include "cube.h"
#include "texture.h"
#include <cstddef>

void makeSphere(Chunk &chunk) {
  for (size_t x = 0; x < CHUNK_SIZE; x++) {
    for (size_t y = 0; y < CHUNK_SIZE; y++) {
      for (size_t z = 0; z < CHUNK_SIZE; z++) {
        auto a = (x - CHUNK_SIZE / 2) * (x - CHUNK_SIZE / 2);
        auto b = (y - CHUNK_SIZE / 2) * (y - CHUNK_SIZE / 2);
        auto c = (z - CHUNK_SIZE / 2) * (z - CHUNK_SIZE / 2);
        auto d = CHUNK_SIZE / 2;
        if (sqrtf(a + b + c) <= d) {
          chunk.cubes[x][y][z].isActive = true;
        }
      }
    }
  }
}

Chunk createChunk(glm::vec3 pos) {
  auto diff =
      loadTexture("resources/beige-textures/beige_wall_001_diff_4k.jpg");
  auto spec =
      loadTexture("resources/beige-textures/beige_wall_001_diff_4k.jpg");
  Chunk chunk;
  chunk.pos = pos;

  for (size_t x = 0; x < CHUNK_SIZE; x++) {
    for (size_t y = 0; y < CHUNK_SIZE; y++) {
      for (size_t z = 0; z < CHUNK_SIZE; z++) {
        Cube cube = createCube(diff, spec, glm::vec3(x, y - 1.f, z),
                               glm::vec3(1.0f), 0.0f, 1.0f);
        cube.isActive = false;
        chunk.cubes[x][y][z] = cube;
      }
    }
  }

  makeSphere(chunk);

  return chunk;
}

void drawChunk(Chunk &chunk, GLuint shader) {
  for (size_t x = 0; x < CHUNK_SIZE; x++)
    for (size_t y = 0; y < CHUNK_SIZE; y++)
      for (size_t z = 0; z < CHUNK_SIZE; z++)
        drawCube(chunk.cubes[x][y][z], shader);
}
