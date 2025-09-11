#include "chunk.h"
#include "game.h"
#include "gamescene.h"
#include "texture.h"
#include <cstddef>

auto pos = glm::vec3(20.0f, 20.0f, -20.0f);

void createChunks() {
  auto diff =
      loadBindlessTexture("resources/textures/beige_wall_001_diff_4k.jpg");

  auto grass = loadBindlessTexture("resources/textures/grass2.jpg");

  int chunks = 16;
  auto xpos = static_cast<size_t>(pos.x);
  auto zpos = static_cast<size_t>(pos.z);
  for (int x = -(chunks / 2); x < chunks / 2; x++)
    for (int z = -(chunks / 2); z < chunks / 2; z++)
      addChunk(grass, 0, glm::vec3(static_cast<float>(x) + pos.x, 0.0f, static_cast<float>(z) + pos.z), glm::vec3(1.0f), 0.0f, 0.1f, Landscape);
/*
  size_t chunkSize = 64, height = 18, width = 64, depth = 64;
  float chunkSizeFloat = static_cast<float>(chunkSize);
  addChunk(diff, 0, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), 0.0f, 1.f, Wall, chunkSize, height, width, depth);
  addChunk(diff, 0, glm::vec3(0.0f, 0.0f, -chunkSizeFloat), glm::vec3(1.0f), 0.0f, 1.f, Wall, chunkSize, height, width, depth);
  addChunk(diff, 0, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 1.f, Wall, chunkSize, height, width, depth);
  addChunk(diff, 0, glm::vec3(chunkSizeFloat, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 1.f, Wall, chunkSize, height, width, depth);
  addChunk(diff, 0, glm::vec3(0.0f, -0.75f * chunkSizeFloat, -chunkSizeFloat), glm::vec3(1.0f), 0.0f, 1.f, Roof, chunkSize, height, width, depth);

  addCube(grass, 0, glm::vec3(5.0f, 0.0f, 5.0f), glm::vec3(1.0f), 0.0f, 1.0f);*/
}

void buildScene() {
  createScene();
  //createChunks();
  auto marble =
      loadBindlessTexture("resources/textures/marble_01_diff_4k.jpg");
  //addPlane(marble, glm::vec3{0.0f}, glm::vec3{1.0f, 0.0f, 0.0f},
    //       90.f, 200.f);
}

int main() {
  start(true, true);
  buildScene();
  run(Fly, pos, false);
  return 0;
}
