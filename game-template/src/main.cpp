#include "chunk.h"
#include "game.h"
#include "gamescene.h"
#include "texture.h"
#include <cstddef>

auto pos = glm::vec3(0.0f, 16.0f, 0.0f);

void buildScene() {
  auto terrain = createTerrain(16, 16, 0.1f); 
  createScene(terrain);
  //addChunk(0, 0, glm::vec3(0.0f), glm::vec3(1.0f), 0.0f, 0.1f, Sphere);
  //addCube(0, 0, glm::vec3(0.0f), glm::vec3(1.0f), 0.0f, 1.0f);
  //addPlane(marble, glm::vec3{0.0f}, glm::vec3{1.0f, 0.0f, 0.0f},
    //       90.f, 200.f);
}

int main() {
  start(true, true);
  buildScene();
  run(Fly, pos, false);
  return 0;
}
