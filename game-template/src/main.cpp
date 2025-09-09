#include "chunk.h"
#include "game.h"
#include "gamescene.h"
#include "texture.h"

void createChunks() {
  auto diff =
      loadBindlessTexture("resources/textures/beige_wall_001_diff_4k.jpg");

  // addCube(diff, 0, glm::vec3(0.0f), glm::vec3(1.0f), 0.0f, 1.f);
  addChunk(diff, 0, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), 0.0f, 1.f, Wall);
  addChunk(diff, 0, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f), 0.0f, 1.f, Wall);
  addChunk(diff, 0, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 1.f, Wall);
  addChunk(diff, 0, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 1.f, Wall);
  addChunk(diff, 0, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f), 0.0f, 1.f, Roof);
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
  run();
  return 0;
}
