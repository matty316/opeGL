#include "chunk.h"
#include "cube.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "shader.h"
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
          chunk.cubes[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z]
              .isActive = true;
        }
      }
    }
  }
}

bool lDefault = false;
void createVerts(Chunk &chunk) {
  for (size_t x = 0; x < CHUNK_SIZE; x++) {
    for (size_t y = 0; y < CHUNK_SIZE; y++) {
      for (size_t z = 0; z < CHUNK_SIZE; z++) {
        if (chunk.cubes[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z]
                .isActive == false) {
          continue;
        }

        bool lXNegative = lDefault;

        if (x > 0)
          lXNegative =
              chunk
                  .cubes[((x - 1) * CHUNK_SIZE * CHUNK_SIZE) + y * CHUNK_SIZE + z]
                  .isActive;

        bool lXPositive = lDefault;

        if (x < CHUNK_SIZE - 1)
          lXPositive =
              chunk
                  .cubes[((x + 1) * CHUNK_SIZE * CHUNK_SIZE) + y * CHUNK_SIZE + z]
                  .isActive;

        bool lYNegative = lDefault;

        if (y > 0)
          lYNegative =
              chunk
                  .cubes[x * CHUNK_SIZE * CHUNK_SIZE + ((y - 1) * CHUNK_SIZE) + z]
                  .isActive;

        bool lYPositive = lDefault;

        if (y < CHUNK_SIZE - 1)
          lYPositive =
              chunk
                  .cubes[x * CHUNK_SIZE * CHUNK_SIZE + ((y + 1) * CHUNK_SIZE) + z]
                  .isActive;

        bool lZNegative = lDefault;

        if (z > 0)
          lZNegative =
              chunk
                  .cubes[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + (z - 1)]
                  .isActive;

        bool lZPositive = lDefault;

        if (z < CHUNK_SIZE - 1)
          lZPositive =
              chunk
                  .cubes[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + (z + 1)]
                  .isActive;

        chunk.cubes[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z].left =
            !lXNegative;
        chunk.cubes[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z].right =
            !lXPositive;
        chunk.cubes[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z].top =
            !lYPositive;
        chunk.cubes[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z].bottom =
            !lYNegative;
        chunk.cubes[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z].back =
            !lZNegative;
        chunk.cubes[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z].front =
            !lZPositive;

        for (auto &vert : cubeVerts(
                 chunk.cubes[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z],
                 x, y, z))
          chunk.vertices.push_back(vert);

        chunk.vertSize +=
            chunk.cubes[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z]
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

  glVertexArrayVertexBuffer(chunk.vao, 0, chunk.vbo, 0, sizeof(GLfloat) * 8);

  glEnableVertexArrayAttrib(chunk.vao, 0);
  glEnableVertexArrayAttrib(chunk.vao, 1);
  glEnableVertexArrayAttrib(chunk.vao, 2);

  glVertexArrayAttribFormat(chunk.vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribFormat(chunk.vao, 1, 3, GL_FLOAT, GL_FALSE,
                            sizeof(GLfloat) * 3);
  glVertexArrayAttribFormat(chunk.vao, 2, 2, GL_FLOAT, GL_FALSE,
                            sizeof(GLfloat) * 6);

  glVertexArrayAttribBinding(chunk.vao, 0, 0);
  glVertexArrayAttribBinding(chunk.vao, 1, 0);
  glVertexArrayAttribBinding(chunk.vao, 2, 0);
}

Chunk createChunk(glm::vec3 pos, glm::vec3 rotation, float angle, float scale) {
  auto diff =
      loadTexture("resources/beige-textures/beige_wall_001_diff_4k.jpg");
  auto spec =
      loadTexture("resources/beige-textures/beige_wall_001_diff_4k.jpg");
  Chunk chunk;
  chunk.pos = pos;
  chunk.rotation = rotation;
  chunk.angle = angle;
  chunk.diff = diff;
  chunk.spec = spec;
  chunk.scale = scale;
  chunk.cubes = new Cube[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];

  for (size_t x = 0; x < CHUNK_SIZE; x++) {
    for (size_t y = 0; y < CHUNK_SIZE; y++) {
      for (size_t z = 0; z < CHUNK_SIZE; z++) {
        Cube cube = createCube(
            diff, spec, glm::vec3(x * scale, (y - 1.f) * scale, z * scale),
            glm::vec3(1.0f), 0.0f, scale, true);
        cube.isActive = false;
        chunk.cubes[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z] = cube;
      }
    }
  }

  makeSphere(chunk);
  createVerts(chunk);
  setupBuffers(chunk);

  return chunk;
}

void drawChunk(Chunk &chunk, GLuint shader) {
  use(shader);
  setInt(shader, "tiling", 1);

  auto model = glm::mat4(1.0f);
  model = glm::translate(model, chunk.pos * chunk.scale *
                                    static_cast<float>(CHUNK_SIZE));
  model = glm::rotate(model, glm::radians(chunk.angle), chunk.rotation);
  model = glm::scale(model, glm::vec3{chunk.scale});
  setMat4(shader, "model", model);

  setInt(shader, "material.diffuse", 0);
  glBindTextureUnit(0, chunk.diff);

  setInt(shader, "material.specular", 1);
  glBindTextureUnit(1, chunk.spec);

  glBindVertexArray(chunk.vao);
  glDrawArrays(GL_TRIANGLES, 0, chunk.vertSize);
  glBindVertexArray(0);
}
