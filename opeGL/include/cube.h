#pragma once

#include <cstdint>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/fwd.hpp>
#include <vector>

enum BlockType {
  Grass, Snow, Water, Dirt
};

struct Cube {
  glm::vec3 pos = glm::vec3(0.0f), rotation = glm::vec3(1.0f);
  float angle = 0.0f, scale = 1.0f;
  size_t diff, spec;
  GLuint vao, vbo;
  size_t vertSize = 0;
  uint32_t x = 0, y = 0, z = 0;
  bool isActive = true, top = true, bottom = true, front = true, back = true, left = true, right = true;
  BlockType blockType = Grass;
};
void setupCubeBuffers(Cube &cube);
std::vector<GLuint> cubeVerts(Cube &cube, size_t xoffset, size_t yoffset, size_t zoffset);
Cube createCube(size_t diff, size_t spec, glm::vec3 pos, glm::vec3 rotation, float angle, float scale, BlockType blockType = Grass, bool deferBuffers = false);
void drawCube(Cube &cube, GLuint shader);
