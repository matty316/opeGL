#include "game.h"
#include "gamescene.h"

void createChunks() {
  size_t size = 16;
  for (size_t x = 0; x < size; x++)
    for (size_t z = 0; z < size; z++)
      addChunk(glm::vec3(static_cast<float>(x), -1.0f, static_cast<float>(z)), glm::vec3(1.0f), 0.0f, 0.1f);
}

void buildScene() {
  createScene();
  createChunks();
  addPlane("resources/marble-textures/marble_01_diff_4k.jpg",
           "resources/marble-textures/marble_01_spec_4k.jpg",
           glm::vec3{0.0f, -2.0f, 0.0f}, glm::vec3{1.0f, 0.0f, 0.0f}, 90.f,
           100.f);
}

int main() {
  start(true);
  buildScene();
  run();
  return 0;
}
