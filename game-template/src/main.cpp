#include "chunk.h"
#include "game.h"
#include "gamescene.h"
#include "texture.h"
#include <cstddef>

auto pos = glm::vec3(20.0f, 20.0f, -20.0f);

void buildScene() {
  createScene();
  //addPlane(marble, glm::vec3{0.0f}, glm::vec3{1.0f, 0.0f, 0.0f},
    //       90.f, 200.f);
}

int main() {
  start(true, true);
  buildScene();
  run(Fly, pos, false);
  return 0;
}
