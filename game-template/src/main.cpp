#include "game.h"
#include "gamescene.h"

void addVoxel(glm::vec3 pos) {
  addCube("resources/beige-textures/beige_wall_001_diff_4k.jpg",
          "resources/beige-textures/beige_wall_001_diff_4k.jpg", pos,
          glm::vec3(1.0f), 0.0f, 1.0f);
}

void buildWall(float depth, bool zAxis) {
  // obvi gotta optimize this
  size_t height = 40, width = 40;
  for (size_t x = 0; x < width; x++)
    for (size_t y = 0; y < height; y++)
      if (zAxis)
        addVoxel(glm::vec3(static_cast<float>(x) - 20.0f, static_cast<float>(y) - 1.0f,
                           depth));
      else
        addVoxel(glm::vec3(depth, static_cast<float>(y) - 1.0f,
                           static_cast<float>(x) - 20.f));
}

void buildScene() {
  createScene();
  buildWall(20.0f, true);
  buildWall(-20.0f, true);
  buildWall(20.0f, false);
  buildWall(-20.0f, false);
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
