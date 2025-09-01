#include "game.h"
#include "gamescene.h"

int main() {
  start();
  createScene();
  //addModel("resources/cup/scene.gltf", glm::vec3{0.0f, -1.0f, 0.0f},
  //         glm::vec3{1.0f}, 0.0f, 0.25f);
  addCube(glm::vec3(0.0f, -1.5f, 0.0f), glm::vec3(1.0f), 0.0f, 1.0f); 
  addPlane("resources/marble-textures/marble_01_diff_4k.jpg",
           "resources/marble-textures/marble_01_spec_4k.jpg",
           glm::vec3{0.0f, -2.0f, 0.0f}, glm::vec3{1.0f, 0.0f, 0.0f}, 90.f,
           100.f);
  run();
  return 0;
}
