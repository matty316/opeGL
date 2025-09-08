#include "chunk.h"
#include "game.h"
#include "gamescene.h"
#include "texture.h"

void createChunks() { 
 auto diff =
      loadBindlessTexture("resources/textures/grass2.jpg");

  //addCube(diff, 0, glm::vec3(0.0f), glm::vec3(1.0f), 0.0f, 1.f); 
  addChunk(diff, 0, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f), 0.0f, 0.2f, Sphere); 
}

void buildScene() {
  createScene();
  createChunks();
 /* addPlane("resources/marble-textures/marble_01_diff_4k.jpg",
           "resources/marble-textures/marble_01_spec_4k.jpg",
           glm::vec3{0.0f, -2.0f, 0.0f}, glm::vec3{1.0f, 0.0f, 0.0f}, 90.f,
           100.f);*/
}

int main() {
  start(true);
  buildScene();
  run();
  return 0;
}
