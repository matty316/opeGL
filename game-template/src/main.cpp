#include "chunk.h"
#include "game.h"
#include "gamescene.h"
#include "texture.h"

void createChunks() {
  auto diff =
      loadBindlessTexture("resources/textures/beige_wall_001_diff_4k.jpg");

  size_t chunkSize = 64, height = 18, width = 64, depth = 64;
  float chunkSizeFloat = static_cast<float>(chunkSize);
  addChunk(diff, 0, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), 0.0f, 1.f, Wall, chunkSize, height, width, depth);
  addChunk(diff, 0, glm::vec3(0.0f, 0.0f, -chunkSizeFloat), glm::vec3(1.0f), 0.0f, 1.f, Wall, chunkSize, height, width, depth);
  addChunk(diff, 0, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 1.f, Wall, chunkSize, height, width, depth);
  addChunk(diff, 0, glm::vec3(chunkSizeFloat, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 1.f, Wall, chunkSize, height, width, depth);
  addChunk(diff, 0, glm::vec3(0.0f, -0.75f * chunkSizeFloat, -chunkSizeFloat), glm::vec3(1.0f), 0.0f, 1.f, Roof, chunkSize, height, width, depth);
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
  run(FPS, glm::vec3(20.0f, 0.0f, -20.0f));
  return 0;
}
