#include "chunk.h"
#include "game.h"
#include "gamescene.h"
#include "texture.h"

void createChunks() {
  auto diff =
      loadBindlessTexture("resources/textures/beige_wall_001_diff_4k.jpg");

  auto grass = loadBindlessTexture("resources/textures/grass2.jpg");

  int chunks = 8;
  for (int x = 0; x < chunks; x++)
    for (int z = 0; z < chunks; z++)
      addChunk(grass, 0, glm::vec3(static_cast<float>(x), 0.0f, -static_cast<float>(z)), glm::vec3(1.0f), 0.0f, 0.1f, Landscape);
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
  createChunks();
  auto marble =
      loadBindlessTexture("resources/textures/marble_01_diff_4k.jpg");
  addPlane(marble, glm::vec3{0.0f}, glm::vec3{1.0f, 0.0f, 0.0f},
           90.f, 200.f);
}

int main() {
  start(true);
  buildScene();
  run(Fly, glm::vec3(20.0f, 70.0f, -20.0f));
  return 0;
}
