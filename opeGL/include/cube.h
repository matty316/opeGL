#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/fwd.hpp>

struct Cube {
  glm::vec3 pos, rotation;
  float angle, scale;
  GLuint diff, spec;
};

void createCube(const char* diffPath, const char* specPath, glm::vec3 pos, glm::vec3 rotation, float angle, float scale);
void drawCubes(GLuint shader);
void setupCubeBuffers();
glm::mat4 cubeModelMatrix(Cube &cube);
