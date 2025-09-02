#include "game.h"
#include "gamescene.h"

void addVoxel(glm::vec3 pos) {
  addCube("resources/beige-textures/beige_wall_001_diff_4k.jpg",
          "resources/beige-textures/beige_wall_001_diff_4k.jpg", pos,
          glm::vec3(1.0f), 0.0f, 1.0f);
}

void buildWall() {
  addVoxel(glm::vec3(0.0f, -1.0f, 20.0f));
  addVoxel(glm::vec3(0.0f, 0.0f, 20.0f));
  addVoxel(glm::vec3(0.0f, 1.0f, 20.0f));
  addVoxel(glm::vec3(0.0f, 2.0f, 20.0f));
  addVoxel(glm::vec3(0.0f, 3.0f, 20.0f));
  addVoxel(glm::vec3(1.0f, -1.0f, 20.0f));
  addVoxel(glm::vec3(1.0f, 0.0f, 20.0f));
  addVoxel(glm::vec3(1.0f, 1.0f, 20.0f));
  addVoxel(glm::vec3(1.0f, 2.0f, 20.0f));
  addVoxel(glm::vec3(1.0f, 3.0f, 20.0f));
  addVoxel(glm::vec3(2.0f, -1.0f, 20.0f));
  addVoxel(glm::vec3(2.0f, 0.0f, 20.0f));
  addVoxel(glm::vec3(2.0f, 1.0f, 20.0f));
  addVoxel(glm::vec3(2.0f, 2.0f, 20.0f));
  addVoxel(glm::vec3(2.0f, 3.0f, 20.0f));
  addVoxel(glm::vec3(3.0f, -1.0f, 20.0f));
  addVoxel(glm::vec3(3.0f, 0.0f, 20.0f));
  addVoxel(glm::vec3(3.0f, 1.0f, 20.0f));
  addVoxel(glm::vec3(3.0f, 2.0f, 20.0f));
  addVoxel(glm::vec3(3.0f, 3.0f, 20.0f));
}

void buildScene() {
  createScene();
  buildWall();
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
