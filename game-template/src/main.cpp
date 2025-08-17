#include "game.h"
#include "gamescene.h"

int main() {
  start();
  createScene();
  addModel("resources/rubber_duck/scene.gltf", glm::vec3{0.0f, -1.0f, 0.0f},
           glm::vec3{1.0f}, 0.0f, 1.f);
  run();
}
