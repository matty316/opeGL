#include "chunk.h"
#include "GLFW/glfw3.h"
#include "camera.h"
#include "cube.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "shader.h"
#include <PerlinNoise.hpp>
#include <barrier>
#include <cstddef>
#include <cstdint>
#include <format>
#include <mutex>
#include <print>
#include <thread>
#include <unordered_map>

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

std::vector<PerChunkData> perChunkData;
std::vector<DrawArraysIndirectCommand> drawCommands;
GLuint drawCommandBuffer, perChunkBuffer;
size_t numVertices = 0;
std::vector<GLuint> terrainVertices;
std::vector<Chunk> terrainChunks;
std::unordered_map<size_t, Chunk> loadedChunks;

void makeSphere(Chunk &chunk, std::vector<Cube> &cubes) {
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

void makeLandscape(Chunk &chunk, std::vector<Cube> &cubes, float freq = 0.01f,
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

void makeWall(Chunk &chunk, std::vector<Cube> &cubes) {
  for (size_t x = 0; x < chunk.width; x++) {
    for (size_t y = 0; y < chunk.height; y++) {
      size_t xpos = x * chunk.chunkSize * chunk.chunkSize,
             ypos = y * chunk.chunkSize;
      cubes[xpos + ypos].isActive = true;
    }
  }
}

void makeRoof(Chunk &chunk, std::vector<Cube> &cubes) {
  chunk.pos.y += chunk.chunkSize;
  for (size_t x = 0; x < chunk.width; x++) {
    for (size_t z = 0; z < chunk.depth; z++) {
      size_t xpos = x * chunk.chunkSize * chunk.chunkSize;
      cubes[xpos + z].isActive = true;
    }
  }
}

void createVerts(Chunk &chunk, std::vector<Cube> &cubes) {
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

        for (auto &vert : cubeVerts(cubes[xpos + ypos + z], x, y, z))
          chunk.vertices.push_back(vert);

        chunk.vertSize += cubes[xpos + ypos + z].vertSize;
      }
    }
  }
}

void setupBuffers(GLuint &vao, GLuint &vbo, std::vector<GLuint> vertices) {
  glCreateVertexArrays(1, &vao);
  glCreateBuffers(1, &vbo);

  glNamedBufferData(vbo, sizeof(GLuint) * vertices.size(), vertices.data(),
                    GL_DYNAMIC_DRAW);

  glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(GLuint));

  glEnableVertexArrayAttrib(vao, 0);

  glVertexArrayAttribFormat(vao, 0, 1, GL_UNSIGNED_INT, GL_FALSE, 0);

  glVertexArrayAttribBinding(vao, 0, 0);
}

Chunk createChunk(size_t diff, size_t spec, glm::vec3 pos, glm::vec3 rotation,
                  float angle, float scale, ChunkType type,
                  bool shouldSetupBuffers, size_t chunkSize, size_t height,
                  size_t width, size_t depth) {
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

  std::vector<Cube> cubes(chunkSize * chunkSize * chunkSize);

  for (size_t x = 0; x < chunkSize; x++) {
    for (size_t y = 0; y < chunkSize; y++) {
      for (size_t z = 0; z < chunkSize; z++) {
        size_t xpos = x * chunkSize * chunkSize, ypos = y * chunkSize;
        Cube cube;
        cube.isActive = false;
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
void createDrawCommand(Chunk &chunk, size_t x, size_t z) {
  DrawArraysIndirectCommand cmd;
  cmd.count = chunk.vertSize;
  cmd.instanceCount = 1;
  cmd.firstVertex = numVertices;
  cmd.baseVertex = 0;
  numVertices += cmd.count;
  drawCommands[x + z] = cmd;
}

void createChunkVertices(Chunk &chunk) {
  for (auto &vert : chunk.vertices) {
    terrainVertices.push_back(vert);
  }
}

void createPerChunkData(Chunk &chunk, size_t x, size_t z) {
  auto model = glm::mat4(1.0f);
  model = glm::translate(model, chunk.pos * chunk.scale *
                                    static_cast<float>(chunk.chunkSize));
  model = glm::rotate(model, glm::radians(chunk.angle), chunk.rotation);
  model = glm::scale(model, glm::vec3{chunk.scale});

  PerChunkData data;
  data.model = model;
  perChunkData[x + z] = data;
}

void createChunks(std::vector<Chunk> &terrainChunks, float posx, float posz,
                  size_t width, size_t depth, size_t start_x, size_t end_x,
                  size_t inner_loop_limit, float scale, std::mutex &mtx) {
  for (size_t x = start_x; x < end_x; x++) {
    for (size_t z = 0; z < inner_loop_limit; z++) {
      Chunk chunk;
      if (loadedChunks.contains(x * width + z)) {
        chunk = loadedChunks[x * width + z];
      } else {
        float xpos = (posx - x + static_cast<float>(width) / 2.0f);
        float zpos = (posz + z - static_cast<float>(depth) / 2.0f);
        chunk = createChunk(0, 0, glm::vec3(xpos, 0.0f, zpos), glm::vec3(1.0f),
                            0.0f, scale, Landscape, false);
      }
      std::lock_guard<std::mutex> lock(mtx);
      loadedChunks[x * width + z] = chunk;
      terrainChunks[x * width + z] = chunk;
      createPerChunkData(chunk, x * width, z);
    }
  }
}

Terrain createTerrain(size_t width, size_t depth, float scale) {
  Terrain terrain;
  // terrain.rotation = rotation;
  // terrain.angle = angle;
  terrain.scale = scale;
  terrain.width = width;
  terrain.depth = depth;
  updateTerrain(terrain, getCameraPos());

  setupBuffers(terrain.vao, terrain.vbo, terrainVertices);

  glCreateBuffers(1, &drawCommandBuffer);
  glNamedBufferStorage(drawCommandBuffer,
                       sizeof(DrawArraysIndirectCommand) * drawCommands.size(),
                       drawCommands.data(), GL_DYNAMIC_STORAGE_BIT);
  glCreateBuffers(1, &perChunkBuffer);
  glNamedBufferStorage(perChunkBuffer,
                       sizeof(PerChunkData) * perChunkData.size(),
                       perChunkData.data(), GL_DYNAMIC_STORAGE_BIT);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, perChunkBuffer);
  return terrain;
}

void drawTerrain(Terrain &terrain, GLuint shader, glm::mat4 vp) {
  use(shader);
  setMat4(shader, "vp", vp);

  glBindVertexArray(terrain.vao);
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);
  glMultiDrawArraysIndirect(GL_TRIANGLES, 0, drawCommands.size(), 0);
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
  glBindVertexArray(0);
}

void updateTerrain(Terrain &terrain, glm::vec3 pos) {
  numVertices = 0;
  terrainChunks.clear();
  drawCommands.clear();
  perChunkData.clear();
  terrainVertices.clear();
  terrainChunks.resize(terrain.width * terrain.depth);
  drawCommands.resize(terrain.width * terrain.depth);
  perChunkData.resize(terrain.width * terrain.depth);

  auto time = glfwGetTime();
  terrain.pos = pos;
  int outer_loop_limit = terrain.width;
  int inner_loop_limit = terrain.depth;
  int num_threads = 8;
  std::mutex mtx;

  std::vector<std::thread> threads;
  int chunk_size = outer_loop_limit / num_threads;

  for (int t = 0; t < num_threads; ++t) {
    int start_i = t * chunk_size;
    int end_i =
        (t == num_threads - 1) ? outer_loop_limit : (t + 1) * chunk_size;
    threads.emplace_back(createChunks, std::ref(terrainChunks), terrain.pos.x,
                         terrain.pos.z, terrain.width, terrain.depth, start_i,
                         end_i, inner_loop_limit, terrain.scale, std::ref(mtx));
  }

  for (std::thread &t : threads) {
    t.join();
  }
  std::println("terrain generated in {} seconds", glfwGetTime() - time);

  std::vector<std::thread> bg;

  for (size_t x = 0; x < terrain.width; x++) {
    for (size_t z = 0; z < terrain.depth; z++) {
      auto chunk = terrainChunks[x * terrain.width + z];
      createChunkVertices(chunk);
      createDrawCommand(chunk, x * terrain.width, z);
    }
  }

  for (std::thread &t : bg) {
    t.join();
  }

  std::println("rest of terrain data generated in {} seconds",
               glfwGetTime() - time);
}

void subTerrainData(Terrain &terrain) {
  glBindVertexArray(terrain.vao);

  auto vertSize = terrainVertices.size() * sizeof(GLfloat);
  glNamedBufferData(terrain.vbo, vertSize, nullptr, GL_DYNAMIC_DRAW);
  glNamedBufferSubData(terrain.vbo, 0, vertSize, terrainVertices.data());

  auto drawBufferSize = drawCommands.size() * sizeof(DrawArraysIndirectCommand);
  glNamedBufferSubData(drawCommandBuffer, 0, drawBufferSize,
                       drawCommands.data());

  auto perChunkBufferSize = perChunkData.size() * sizeof(PerChunkData);
  glNamedBufferSubData(perChunkBuffer, 0, perChunkBufferSize,
                       perChunkData.data());
}
