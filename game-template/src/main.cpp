#include "game.h"
#include "gamescene.h"

int main() {
  start();
  createScene();
  addModel("resources/rubberduck.mesh", glm::vec3{0.0f, -1.0f, 0.0f},
           glm::vec3{1.0f}, 0.0f, 1.f);
  run();
}
