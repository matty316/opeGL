#include "chunk.h"
#include "cube.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "shader.h"
#include <PerlinNoise.hpp>
#include <cstddef>
#include <format>
#include <print>

const siv::PerlinNoise::seed_type seed = 6969420;
const siv::PerlinNoise perlin{seed};

void makeSphere(Chunk &chunk, Cube *cubes) {
  for (size_t x = 0; x < chunk.chunkSize; x++) {
    for (size_t y = 0; y < chunk.chunkSize; y++) {
      for (size_t z = 0; z < chunk.chunkSize; z++) {
        auto a = (x - chunk.chunkSize / 2) * (x - chunk.chunkSize / 2);
        auto b = (y - chunk.chunkSize / 2) * (y - chunk.chunkSize / 2);
        auto c = (z - chunk.chunkSize / 2) * (z - chunk.chunkSize / 2);
        auto d = chunk.chunkSize / 2;
        if (sqrtf(a + b + c) <= d) {
          cubes[x * chunk.chunkSize * chunk.chunkSize + y * chunk.chunkSize + z]
              .isActive = true;
        }
      }
    }
  }
}

void makeLandscape(Chunk &chunk, Cube *cubes, float freq = 0.01f,
                   int octave = 8) {
  std::println("generating terrain");
  for (size_t x = 0; x < chunk.chunkSize; x++) {
    for (size_t y = 0; y < chunk.chunkSize; y++) {
      for (size_t z = 0; z < chunk.chunkSize; z++) {
        float yFloat = static_cast<float>(y),
              chunkSize = static_cast<float>(chunk.chunkSize);
        if (yFloat < chunkSize * 0.2f && yFloat >= chunkSize * 0.2f - 4.0f) {
          cubes[x * chunk.chunkSize * chunk.chunkSize + y * chunk.chunkSize + z]
              .blockType = Water;
          cubes[x * chunk.chunkSize * chunk.chunkSize + y * chunk.chunkSize + z]
              .isActive = true;
        } else if (yFloat < chunkSize * 0.2f - 4.0f) {
          cubes[x * chunk.chunkSize * chunk.chunkSize + y * chunk.chunkSize + z]
              .blockType = Dirt;
          cubes[x * chunk.chunkSize * chunk.chunkSize + y * chunk.chunkSize + z]
              .isActive = true;
        }
      }
    }
  }

  for (size_t x = 0; x < chunk.chunkSize; x++) {
    for (size_t z = 0; z < chunk.chunkSize; z++) {
      const float noise =
          perlin.octave2D_01(
              (static_cast<float>(x) + chunk.pos.x * chunk.chunkSize) * freq,
              (static_cast<float>(z) + chunk.pos.z * chunk.chunkSize) * freq,
              octave) *
          chunk.chunkSize;
      for (size_t y = 0; y < static_cast<size_t>(noise); y++) {
        cubes[x * chunk.chunkSize * chunk.chunkSize + y * chunk.chunkSize + z]
            .isActive = true;
        if (static_cast<float>(y) > static_cast<float>(chunk.chunkSize) * 0.7f)
          cubes[x * chunk.chunkSize * chunk.chunkSize + y * chunk.chunkSize + z]
              .blockType = Snow;
      }
    }
  }
}

void makeWall(Chunk &chunk, Cube *cubes) {
  for (size_t x = 0; x < chunk.width; x++)
    for (size_t y = 0; y < chunk.height; y++)
      cubes[x * chunk.chunkSize * chunk.chunkSize + y * chunk.chunkSize]
          .isActive = true;
}

void makeRoof(Chunk &chunk, Cube *cubes) {
  chunk.pos.y += chunk.chunkSize;
  for (size_t x = 0; x < chunk.width; x++)
    for (size_t z = 0; z < chunk.depth; z++)
      cubes[x * chunk.chunkSize * chunk.chunkSize + z].isActive = true;
}

void createVerts(Chunk &chunk, Cube *cubes) {
  bool lDefault = false;
  for (size_t x = 0; x < chunk.chunkSize; x++) {
    for (size_t y = 0; y < chunk.chunkSize; y++) {
      for (size_t z = 0; z < chunk.chunkSize; z++) {
        if (cubes[x * chunk.chunkSize * chunk.chunkSize + y * chunk.chunkSize +
                  z]
                .isActive == false) {
          continue;
        }

        bool lXNegative = lDefault;

        if (x > 0)
          lXNegative =

              cubes[((x - 1) * chunk.chunkSize * chunk.chunkSize) +
                    y * chunk.chunkSize + z]
                  .isActive;

        bool lXPositive = lDefault;

        if (x < chunk.chunkSize - 1)
          lXPositive =

              cubes[((x + 1) * chunk.chunkSize * chunk.chunkSize) +
                    y * chunk.chunkSize + z]
                  .isActive;

        bool lYNegative = lDefault;

        if (y > 0)
          lYNegative = cubes[x * chunk.chunkSize * chunk.chunkSize +
                             ((y - 1) * chunk.chunkSize) + z]
                           .isActive;

        bool lYPositive = lDefault;

        if (y < chunk.chunkSize - 1)
          lYPositive = cubes[x * chunk.chunkSize * chunk.chunkSize +
                             ((y + 1) * chunk.chunkSize) + z]
                           .isActive;

        bool lZNegative = lDefault;

        if (z > 0)
          lZNegative = cubes[x * chunk.chunkSize * chunk.chunkSize +
                             y * chunk.chunkSize + (z - 1)]
                           .isActive;

        bool lZPositive = lDefault;

        if (z < chunk.chunkSize - 1)
          lZPositive = cubes[x * chunk.chunkSize * chunk.chunkSize +
                             y * chunk.chunkSize + (z + 1)]
                           .isActive;

        cubes[x * chunk.chunkSize * chunk.chunkSize + y * chunk.chunkSize + z]
            .left = !lXNegative;

        cubes[x * chunk.chunkSize * chunk.chunkSize + y * chunk.chunkSize + z]
            .right = !lXPositive;

        cubes[x * chunk.chunkSize * chunk.chunkSize + y * chunk.chunkSize + z]
            .top = !lYPositive;

        cubes[x * chunk.chunkSize * chunk.chunkSize + y * chunk.chunkSize + z]
            .bottom = !lYNegative;

        cubes[x * chunk.chunkSize * chunk.chunkSize + y * chunk.chunkSize + z]
            .back = !lZNegative;
        cubes[x * chunk.chunkSize * chunk.chunkSize + y * chunk.chunkSize + z]
            .front = !lZPositive;

        for (auto &vert :
             cubeVerts(cubes[x * chunk.chunkSize * chunk.chunkSize +
                             y * chunk.chunkSize + z],
                       x, y, z))
          chunk.vertices.push_back(vert);

        chunk.vertSize += cubes[x * chunk.chunkSize * chunk.chunkSize +
                                y * chunk.chunkSize + z]
                              .vertSize;
      }
    }
  }
}

void setupBuffers(Chunk &chunk) {
  glCreateVertexArrays(1, &chunk.vao);
  glCreateBuffers(1, &chunk.vbo);

  glNamedBufferStorage(chunk.vbo, sizeof(GLfloat) * chunk.vertices.size(),
                       chunk.vertices.data(), GL_DYNAMIC_STORAGE_BIT);

  glVertexArrayVertexBuffer(chunk.vao, 0, chunk.vbo, 0, sizeof(GLfloat) * 11);

  glEnableVertexArrayAttrib(chunk.vao, 0);
  glEnableVertexArrayAttrib(chunk.vao, 1);
  glEnableVertexArrayAttrib(chunk.vao, 2);
  glEnableVertexArrayAttrib(chunk.vao, 3);

  glVertexArrayAttribFormat(chunk.vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribFormat(chunk.vao, 1, 3, GL_FLOAT, GL_FALSE,
                            sizeof(GLfloat) * 3);
  glVertexArrayAttribFormat(chunk.vao, 2, 2, GL_FLOAT, GL_FALSE,
                            sizeof(GLfloat) * 6);
  glVertexArrayAttribFormat(chunk.vao, 3, 3, GL_FLOAT, GL_FALSE,
                            sizeof(GLfloat) * 8);

  glVertexArrayAttribBinding(chunk.vao, 0, 0);
  glVertexArrayAttribBinding(chunk.vao, 1, 0);
  glVertexArrayAttribBinding(chunk.vao, 2, 0);
  glVertexArrayAttribBinding(chunk.vao, 3, 0);
}

Chunk createChunk(size_t diff, size_t spec, glm::vec3 pos, glm::vec3 rotation,
                  float angle, float scale, ChunkType type, size_t chunkSize,
                  size_t height, size_t width, size_t depth) {
  if (height > chunkSize || width > chunkSize || depth > chunkSize) {
    std::println("height, width and depth must be lower than chunk size");
    exit(1);
  }

  Chunk chunk;
  chunk.pos = pos;
  chunk.rotation = rotation;
  chunk.angle = angle;
  chunk.diff = diff;
  chunk.spec = spec;
  chunk.scale = scale;
  chunk.chunkSize = chunkSize;
  chunk.height = height == 0 ? chunkSize : height;
  chunk.width = width == 0 ? chunkSize : width;
  chunk.depth = depth == 0 ? chunkSize : depth;

  Cube *cubes = new Cube[chunk.chunkSize * chunk.chunkSize * chunk.chunkSize];

  for (size_t x = 0; x < chunk.chunkSize; x++) {
    for (size_t y = 0; y < chunk.chunkSize; y++) {
      for (size_t z = 0; z < chunk.chunkSize; z++) {
        Cube cube =
            createCube(diff, spec, glm::vec3(x * scale, y * scale, z * scale),
                       glm::vec3(1.0f), 0.0f, scale, Grass, true);
        cube.isActive = false;
        cube.x = x;
        cube.y = y;
        cube.z = z;
        cubes[x * chunk.chunkSize * chunk.chunkSize + y * chunk.chunkSize + z] =
            cube;
      }
    }
  }

  switch (type) {
  case Sphere:
    makeSphere(chunk, cubes);
    break;
  case Landscape:
    makeLandscape(chunk, cubes);
    break;
  case Wall:
    makeWall(chunk, cubes);
    break;
  case Roof:
    makeRoof(chunk, cubes);
    break;
  }

  createVerts(chunk, cubes);
  delete[] cubes;
  setupBuffers(chunk);

  return chunk;
}

void drawChunk(Chunk &chunk, GLuint shader) {
  use(shader);
  setInt(shader, "tiling", 1);

  auto model = glm::mat4(1.0f);
  model = glm::translate(model, chunk.pos * chunk.scale *
                                    static_cast<float>(chunk.chunkSize));
  model = glm::rotate(model, glm::radians(chunk.angle), chunk.rotation);
  model = glm::scale(model, glm::vec3{chunk.scale});
  setMat4(shader, "model", model);

  setInt(shader, "textureIndex", chunk.diff);

  glBindVertexArray(chunk.vao);
  glDrawArrays(GL_TRIANGLES, 0, chunk.vertSize);
  glBindVertexArray(0);
}
