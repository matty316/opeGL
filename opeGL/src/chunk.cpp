#include "chunk.h"
#include "camera.h"
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

struct DrawArraysIndirectCommand {
  GLuint count;
  GLuint instanceCount;
  GLuint firstVertex;
  GLuint baseVertex;
};

struct PerChunkData {
  glm::mat4 model;
};

std::vector<DrawArraysIndirectCommand> drawCommands;
GLuint drawCommandBuffer, perChunkBuffer;

void makeSphere(Chunk &chunk, Cube *cubes) {
  for (size_t x = 0; x < chunk.chunkSize; x++) {
    for (size_t y = 0; y < chunk.chunkSize; y++) {
      for (size_t z = 0; z < chunk.chunkSize; z++) {
        auto a = (x - chunk.chunkSize / 2) * (x - chunk.chunkSize / 2);
        auto b = (y - chunk.chunkSize / 2) * (y - chunk.chunkSize / 2);
        auto c = (z - chunk.chunkSize / 2) * (z - chunk.chunkSize / 2);
        auto d = chunk.chunkSize / 2;
        if (sqrtf(a + b + c) <= d) {
          size_t xpos = x * chunk.chunkSize * chunk.chunkSize,
                 ypos = y * chunk.chunkSize;
          cubes[xpos + ypos + z].isActive = true;
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
        size_t xpos = x * chunk.chunkSize * chunk.chunkSize,
               ypos = y * chunk.chunkSize;
        float yFloat = static_cast<float>(y),
              chunkSize = static_cast<float>(chunk.chunkSize);
        float waterLevel = 0.3f;
        if (yFloat < chunkSize * waterLevel &&
            yFloat >= chunkSize * 0.2f - 4.0f) {
          cubes[xpos + ypos + z].blockType = Water;
          cubes[xpos + ypos + z].isActive = true;
        } else if (yFloat < chunkSize * waterLevel - 4.0f) {
          cubes[xpos + ypos + z].blockType = Dirt;
          cubes[xpos + ypos + z].isActive = true;
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
        size_t xpos = x * chunk.chunkSize * chunk.chunkSize,
               ypos = y * chunk.chunkSize;
        cubes[xpos + ypos + z].isActive = true;
        if (static_cast<float>(y) > static_cast<float>(chunk.chunkSize) * 0.7f)
          cubes[xpos + ypos + z].blockType = Snow;
      }
    }
  }
}

void makeWall(Chunk &chunk, Cube *cubes) {
  for (size_t x = 0; x < chunk.width; x++) {
    for (size_t y = 0; y < chunk.height; y++) {
      size_t xpos = x * chunk.chunkSize * chunk.chunkSize,
             ypos = y * chunk.chunkSize;
      cubes[xpos + ypos].isActive = true;
    }
  }
}

void makeRoof(Chunk &chunk, Cube *cubes) {
  chunk.pos.y += chunk.chunkSize;
  for (size_t x = 0; x < chunk.width; x++) {
    for (size_t z = 0; z < chunk.depth; z++) {
      size_t xpos = x * chunk.chunkSize * chunk.chunkSize;
      cubes[xpos + z].isActive = true;
    }
  }
}

void createVerts(Chunk &chunk, Cube *cubes) {
  bool lDefault = false;
  for (size_t x = 0; x < chunk.chunkSize; x++) {
    for (size_t y = 0; y < chunk.chunkSize; y++) {
      for (size_t z = 0; z < chunk.chunkSize; z++) {

        size_t xpos = x * chunk.chunkSize * chunk.chunkSize,
               ypos = y * chunk.chunkSize;

        if (cubes[xpos + ypos + z].isActive == false) {
          continue;
        }

        bool lXNegative = lDefault;

        if (x > 0)
          lXNegative =

              cubes[((x - 1) * chunk.chunkSize * chunk.chunkSize) + ypos + z]
                  .isActive;

        bool lXPositive = lDefault;

        if (x < chunk.chunkSize - 1)
          lXPositive =

              cubes[((x + 1) * chunk.chunkSize * chunk.chunkSize) + ypos + z]
                  .isActive;

        bool lYNegative = lDefault;

        if (y > 0)
          lYNegative = cubes[xpos + ((y - 1) * chunk.chunkSize) + z].isActive;

        bool lYPositive = lDefault;

        if (y < chunk.chunkSize - 1)
          lYPositive = cubes[xpos + ((y + 1) * chunk.chunkSize) + z].isActive;

        bool lZNegative = lDefault;

        if (z > 0)
          lZNegative = cubes[xpos + ypos + (z - 1)].isActive;

        bool lZPositive = lDefault;

        if (z < chunk.chunkSize - 1)
          lZPositive = cubes[xpos + ypos + (z + 1)].isActive;

        cubes[xpos + ypos + z].left = !lXNegative;
        cubes[xpos + ypos + z].right = !lXPositive;
        cubes[xpos + ypos + z].top = !lYPositive;
        cubes[xpos + ypos + z].bottom = !lYNegative;
        cubes[xpos + ypos + z].back = !lZNegative;
        cubes[xpos + ypos + z].front = !lZPositive;

        if (x == 0)
          cubes[xpos + ypos + z].left = false;
        if (x == chunk.chunkSize - 1)
          cubes[xpos + ypos + z].right = false;
        if (y == 0)
          cubes[xpos + ypos + z].bottom = false;
        if (z == 0)
          cubes[xpos + ypos + z].back = false;
        if (z == chunk.chunkSize - 1)
          cubes[xpos + ypos + z].front = false;

        for (auto &vert : cubeVerts(cubes[xpos + ypos + z], x + chunk.xoffset,
                                    y, z + chunk.zoffset))
          chunk.vertices.push_back(vert);

        chunk.vertSize += cubes[xpos + ypos + z].vertSize;
      }
    }
  }
}

void setupBuffers(GLuint &vao, GLuint &vbo, std::vector<GLfloat> vertices) {
  glCreateVertexArrays(1, &vao);
  glCreateBuffers(1, &vbo);

  glNamedBufferStorage(vbo, sizeof(GLfloat) * vertices.size(), vertices.data(),
                       GL_DYNAMIC_STORAGE_BIT);

  glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(GLfloat) * 9);

  glEnableVertexArrayAttrib(vao, 0);
  glEnableVertexArrayAttrib(vao, 1);
  glEnableVertexArrayAttrib(vao, 2);

  glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3);
  glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6);

  glVertexArrayAttribBinding(vao, 0, 0);
  glVertexArrayAttribBinding(vao, 1, 0);
  glVertexArrayAttribBinding(vao, 2, 0);
}

Chunk createChunk(size_t diff, size_t spec, glm::vec3 pos, glm::vec3 rotation,
                  float angle, float scale, ChunkType type,
                  bool shouldSetupBuffers, size_t xoffset, size_t zoffset,
                  size_t chunkSize, size_t height, size_t width, size_t depth) {
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
  chunk.xoffset = xoffset;
  chunk.zoffset = zoffset;

  Cube *cubes = new Cube[chunk.chunkSize * chunk.chunkSize * chunk.chunkSize];

  for (size_t x = 0; x < chunk.chunkSize; x++) {
    for (size_t y = 0; y < chunk.chunkSize; y++) {
      for (size_t z = 0; z < chunk.chunkSize; z++) {
        size_t xpos = x * chunk.chunkSize * chunk.chunkSize,
               ypos = y * chunk.chunkSize;
        Cube cube = createCube(diff, spec, glm::vec3(0.0f), glm::vec3(1.0f),
                               0.0f, 1.0f, Grass, true);
        cube.isActive = false;
        cube.x = x;
        cube.y = y;
        cube.z = z;
        cubes[xpos + ypos + z] = cube;
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
  std::println("size of verts: {}", sizeof(GLfloat) * chunk.vertices.size());
  delete[] cubes;
  if (shouldSetupBuffers)
    setupBuffers(chunk.vao, chunk.vbo, chunk.vertices);

  return chunk;
}

void drawChunk(Chunk &chunk, GLuint shader, glm::mat4 vp) {
  use(shader);

  auto model = glm::mat4(1.0f);
  model = glm::translate(model, chunk.pos * chunk.scale *
                                    static_cast<float>(chunk.chunkSize));
  model = glm::rotate(model, glm::radians(chunk.angle), chunk.rotation);
  model = glm::scale(model, glm::vec3{chunk.scale});

  setMat4(shader, "model", model);
  setMat4(shader, "vp", vp);

  glBindVertexArray(chunk.vao);
  glDrawArrays(GL_TRIANGLES, 0, chunk.vertSize);
  glBindVertexArray(0);
}

Terrain createTerrain(size_t width, size_t depth) {
  Terrain terrain;
  terrain.pos = getCameraPos();
  // terrain.rotation = rotation;
  // terrain.angle = angle;
  // terrain.scale = scale;
  terrain.width = width;
  terrain.depth = depth;

  size_t numVertices = 0;
  std::vector<GLfloat> vertices;
  for (size_t x = 0; x < width; x++) {
    for (size_t z = 0; z < depth; z++) {

      Chunk chunk =
          createChunk(0, 0,
                      glm::vec3(static_cast<float>(x) + terrain.pos.x, 0.0f,
                                -static_cast<float>(z) + terrain.pos.z),
                      glm::vec3(1.0f), 0.0f, 0.1f, Landscape, false);
      terrain.chunks.push_back(chunk);

      for (auto &vert : chunk.vertices)
        vertices.push_back(vert);

      DrawArraysIndirectCommand cmd;
      cmd.count = chunk.vertSize;
      cmd.instanceCount = 1;
      cmd.firstVertex = numVertices;
      cmd.baseVertex = 0;
      drawCommands.push_back(cmd);
      numVertices += cmd.count;
    }
  }

  setupBuffers(terrain.vao, terrain.vbo, vertices);

  glCreateBuffers(1, &drawCommandBuffer);
  glNamedBufferStorage(drawCommandBuffer,
                       sizeof(DrawArraysIndirectCommand) * drawCommands.size(),
                       drawCommands.data(), GL_DYNAMIC_STORAGE_BIT);

  glCreateBuffers(1, &perChunkBuffer);
  glNamedBufferStorage(perChunkBuffer,
                       sizeof(PerChunkData) * terrain.chunks.size(), nullptr,
                       GL_DYNAMIC_STORAGE_BIT);

  return terrain;
}

void drawTerrain(Terrain &terrain, GLuint shader, glm::mat4 vp) {
  use(shader);
  setMat4(shader, "vp", vp);

  std::vector<PerChunkData> perChunkData;
  for (auto &chunk : terrain.chunks) {
      auto model = glm::mat4(1.0f);
      model = glm::translate(model, chunk.pos * chunk.scale *
                                        static_cast<float>(chunk.chunkSize));
      model = glm::rotate(model, glm::radians(chunk.angle), chunk.rotation);
      model = glm::scale(model, glm::vec3{chunk.scale});

      PerChunkData data;
      data.model = model;
      perChunkData.push_back(data);
  }
  glNamedBufferSubData(perChunkBuffer, 0,
                       sizeof(PerChunkData) * perChunkData.size(),
                       perChunkData.data());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, perChunkBuffer);

  glBindVertexArray(terrain.vao);
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);
  glMultiDrawArraysIndirect(GL_TRIANGLES, 0, drawCommands.size(), 0);
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
  glBindVertexArray(0);
}
