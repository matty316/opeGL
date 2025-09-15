#include "cube.h"
#include "glm/fwd.hpp"
#include "shader.h"
#include <cstddef>
#include <cstdint>
#include <glm/ext/matrix_transform.hpp>
#include <ios>
#include <iostream>
#include <ostream>
#include <print>
#include <vector>

int8_t cubeBackFace[] = {
    -1, -1, -1, 0, // Bottom-left
    1,  1,  -1, 0, // top-right
    1,  -1, -1, 0, // bottom-right
    1,  1,  -1, 0, // top-right
    -1, -1, -1, 0, // bottom-left
    -1, 1,  -1, 0, // top-left
};

int8_t cubeFrontFace[] = {
    -1, -1, 1, 1, // bottom-left
    1,  -1, 1, 1, // bottom-right
    1,  1,  1, 1, // top-right
    1,  1,  1, 1, // top-right
    -1, 1,  1, 1, // top-left
    -1, -1, 1, 1, // bottom-left
};

int8_t cubeLeftFace[] = {
    -1, 1,  1,  2, // top-right
    -1, 1,  -1, 2, // top-left
    -1, -1, -1, 2, // bottom-left
    -1, -1, -1, 2, // bottom-left
    -1, -1, 1,  2, // bottom-right
    -1, 1,  1,  2, // top-right
};

int8_t cubeRightFace[] = {
    1, 1,  1,  3, // top-left
    1, -1, -1, 3, // bottom-right
    1, 1,  -1, 3, // top-right
    1, -1, -1, 3, // bottom-right
    1, 1,  1,  3, // top-left
    1, -1, 1,  3, // bottom-left
};

int8_t cubeBottomFace[] = {
    -1, -1, -1, 4, // top-right
    1,  -1, -1, 4, // top-left
    1,  -1, 1,  4, // bottom-left
    1,  -1, 1,  4, // bottom-left
    -1, -1, 1,  4, // bottom-right
    -1, -1, -1, 4, // top-right
};

int8_t cubeTopFace[] = {
    -1, 1, -1, 5, // top-left
    1,  1, 1,  5, // bottom-right
    1,  1, -1, 5, // top-right
    1,  1, 1,  5, // bottom-right
    -1, 1, -1, 5, // top-left
    -1, 1, 1,  5, // bottom-left
};

int8_t grassColor = 0; //{0.427f, 0.549f, 0.196f};
int8_t snowColor = 1;  //{1.0f, 0.957f, 0.878f};
int8_t waterColor = 2; //{0.271f, 0.486f, 0.839f};
int8_t dirtColor = 3;  //{0.173f, 0.106f, 0.18f};

uint32_t packSignedInts(int8_t i1, int8_t i2, int8_t i3, int8_t i4) {
  uint32_t packed_value = 0;

  // Cast to unsigned char to handle sign extension during bitwise operations
  packed_value |= static_cast<uint8_t>(i1) << 24; // Most significant byte
  packed_value |= static_cast<uint8_t>(i2) << 16;
  packed_value |= static_cast<uint8_t>(i3) << 8;
  packed_value |= static_cast<uint8_t>(i4); // Least significant byte

  return packed_value;
}

const size_t vertBufferRowSize = 4, numOfVertRows = 6,
             totalVertBufferRowSize = 4;
std::vector<GLuint>
vertsWithOffset(Cube &cube, int8_t oldVerts[vertBufferRowSize * numOfVertRows],
                size_t xoffset, size_t yoffset, size_t zoffset) {
  std::vector<GLuint> verts;
  std::vector<int32_t> transformedVerts;

  for (size_t i = 0; i < numOfVertRows; i++) {
    for (size_t j = 0; j < vertBufferRowSize; j++)
      transformedVerts.push_back(oldVerts[i * vertBufferRowSize + j]);
  }

  for (size_t i = 0; i < numOfVertRows; i++) {
    transformedVerts[i * totalVertBufferRowSize] += xoffset;
    transformedVerts[i * totalVertBufferRowSize + 1] += yoffset;
    transformedVerts[i * totalVertBufferRowSize + 2] += zoffset;
  }

  for (size_t i = 0; i < numOfVertRows; i++) {
    auto data =
        packSignedInts(transformedVerts[i * totalVertBufferRowSize],
                       transformedVerts[i * totalVertBufferRowSize + 1],
                       transformedVerts[i * totalVertBufferRowSize + 2],
                       transformedVerts[i * totalVertBufferRowSize + 3]);
    // uint32_t data = (color << 21) | (norm << 18) | (z << 12) | (y
    // << 6) | x;
    std::cout << std::hex << data << std::endl;
    verts.push_back(data);
  }

  return verts;
}

std::vector<GLuint> cubeVerts(Cube &cube, size_t xoffset, size_t yoffset,
                              size_t zoffset) {
  if (!cube.isActive)
    return {};

  std::vector<GLuint> verts;
  size_t vertSize = 0;
  const size_t vertsPerRow = 6;
  if (cube.back) {
    auto newVerts =
        vertsWithOffset(cube, cubeBackFace, xoffset, yoffset, zoffset);
    for (auto &vert : newVerts)
      verts.push_back(vert);
    vertSize += vertsPerRow;
  }
  if (cube.front) {
    auto newVerts =
        vertsWithOffset(cube, cubeFrontFace, xoffset, yoffset, zoffset);
    for (auto &vert : newVerts)
      verts.push_back(vert);
    vertSize += vertsPerRow;
  }
  if (cube.left) {
    auto newVerts =
        vertsWithOffset(cube, cubeLeftFace, xoffset, yoffset, zoffset);
    for (auto &vert : newVerts)
      verts.push_back(vert);
    vertSize += vertsPerRow;
  }
  if (cube.right) {
    auto newVerts =
        vertsWithOffset(cube, cubeRightFace, xoffset, yoffset, zoffset);
    for (auto &vert : newVerts)
      verts.push_back(vert);
    vertSize += vertsPerRow;
  }
  if (cube.bottom) {
    auto newVerts =
        vertsWithOffset(cube, cubeBottomFace, xoffset, yoffset, zoffset);
    for (auto &vert : newVerts)
      verts.push_back(vert);
    vertSize += vertsPerRow;
  }
  if (cube.top) {
    auto newVerts =
        vertsWithOffset(cube, cubeTopFace, xoffset, yoffset, zoffset);
    for (auto &vert : newVerts)
      verts.push_back(vert);
    vertSize += vertsPerRow;
  }

  cube.vertSize = vertSize;

  return verts;
}

void setupCubeBuffers(Cube &cube) {
  auto verts = cubeVerts(cube, 0, 0, 0);

  if (verts.size() == 0)
    return;

  glCreateVertexArrays(1, &cube.vao);
  glCreateBuffers(1, &cube.vbo);

  glNamedBufferStorage(cube.vbo, sizeof(GLint) * verts.size(), verts.data(),
                       GL_DYNAMIC_STORAGE_BIT);

  glVertexArrayVertexBuffer(cube.vao, 0, cube.vbo, 0, sizeof(GLint));

  glEnableVertexArrayAttrib(cube.vao, 0);

  glVertexArrayAttribFormat(cube.vao, 0, 1, GL_INT, GL_FALSE, 0);

  glVertexArrayAttribBinding(cube.vao, 0, 0);
}

Cube createCube(size_t diff, size_t spec, glm::vec3 pos, glm::vec3 rotation,
                float angle, float scale, BlockType blockType,
                bool deferBuffers) {
  Cube cube;
  cube.pos = pos;
  cube.rotation = rotation;
  cube.angle = angle;
  cube.scale = scale;
  cube.blockType = blockType;
  cube.diff = diff;
  cube.spec = spec;

  if (!deferBuffers)
    setupCubeBuffers(cube);

  return cube;
}

glm::mat4 cubeModelMatrix(Cube &cube) {
  auto model = glm::mat4{1};
  model = glm::translate(model, cube.pos);
  model = glm::rotate(model, glm::radians(cube.angle), cube.rotation);
  model = glm::scale(model, glm::vec3{cube.scale});
  return model;
}

void drawCube(Cube &cube, GLuint shader) {
  if (!cube.isActive || cube.vertSize == 0)
    return;

  use(shader);
  setInt(shader, "tiling", 1);

  auto model = cubeModelMatrix(cube);
  setMat4(shader, "model", model);

  glBindVertexArray(cube.vao);
  glDrawArrays(GL_TRIANGLES, 0, cube.vertSize);
  glBindVertexArray(0);
}
