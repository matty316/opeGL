#include "game.h"
#include "gamescene.h"

void addVoxel(glm::vec3 pos) {
  addCube("resources/beige-textures/beige_wall_001_diff_4k.jpg",
          "resources/beige-textures/beige_wall_001_diff_4k.jpg", pos,
          glm::vec3(1.0f), 0.0f, 1.0f);
}

void buildWall(float z) {
  //obvi gotta optimize this
  size_t height = 5, width = 5;
  for (size_t x = 0; x < width; x++)
    for (size_t y = 0; y < height; y++)
      addVoxel(glm::vec3(static_cast<float>(x), static_cast<float>(y) - 1.0f, z));
 }

void buildScene() {
  createScene();
  buildWall(5.0f);
  buildWall(-5.0f);
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
