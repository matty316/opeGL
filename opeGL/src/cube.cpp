#include "cube.h"
#include "glm/fwd.hpp"
#include "shader.h"
#include <cstddef>
#include <cstdint>
#include <glm/ext/matrix_transform.hpp>
#include <vector>

GLuint cubeBackFace[] = {
    0x0000, //-1, -1, -1, 0, // Bottom-left
    0x1100, // 1,  1, -1, 0, // top-right
    0x1000, // 1, -1, -1, 0, // bottom-right
    0x1100, // 1,  1, -1, 0, // top-right
    0x0000, //-1, -1, -1, 0, // bottom-left
    0x0100, //-1,  1, -1, 0, // top-left
};

GLuint cubeFrontFace[] = {
    0x0011, //-1, -1,  1, 1, // bottom-left
    0x1011, // 1, -1,  1, 1, // bottom-right
    0x1111, // 1,  1,  1, 1, // top-right
    0x1111, // 1,  1,  1, 1, // top-right
    0x0111, //-1,  1,  1, 1, // top-left
    0x0011, //-1, -1,  1, 1, // bottom-left
};

GLuint cubeLeftFace[] = {
    0x0112, //-1,  1,  1, 2, // top-right
    0x0102, //-1,  1, -1, 2, // top-left
    0x0002, //-1, -1, -1, 2, // bottom-left
    0x0002, //-1, -1, -1, 2, // bottom-left
    0x0012, //-1, -1,  1, 2, // bottom-right
    0x0112, //-1,  1,  1, 2,  // top-right
};

GLuint cubeRightFace[] = {
    0x1113, //1, 1,  1,  3, // top-left
    0x1003, //1, -1, -1, 3, // bottom-right
    0x1103, //1, 1,  -1, 3, // top-right
    0x1003, //1, -1, -1, 3, // bottom-right
    0x1113, //1, 1,  1,  3, // top-left
    0x1013, //1, -1, 1,  3, // bottom-left
};

GLuint cubeBottomFace[] = {
    0x0004, //-1, -1, -1, 4, // top-right
    0x1004, //1,  -1, -1, 4, // top-left
    0x1014, //1,  -1, 1,  4, // bottom-left
    0x1014, //1,  -1, 1,  4, // bottom-left
    0x0014, //-1, -1, 1,  4, // bottom-right
    0x0004, //-1, -1, -1, 4, // top-right
};

GLuint cubeTopFace[] = {
    0x0105, //-1, 1, -1, 5, // top-left
    0x1115, //1,  1, 1,  5, // bottom-right
    0x1105, //1,  1, -1, 5, // top-right
    0x1115, //1,  1, 1,  5, // bottom-right
    0x0105, //-1, 1, -1, 5, // top-left
    0x0115, //-1, 1, 1,  5, // bottom-left
};

GLuint grassColor = 0; //{0.427f, 0.549f, 0.196f};
GLuint snowColor = 1;  //{1.0f, 0.957f, 0.878f};
GLuint waterColor = 2; //{0.271f, 0.486f, 0.839f};
GLuint dirtColor = 3;  //{0.173f, 0.106f, 0.18f};

const size_t vertBufferRowSize = 4, numOfVertRows = 6,
             totalVertBufferRowSize = 5;
std::vector<GLuint> vertsWithOffset(Cube &cube, int8_t oldVerts[6], int8_t xoffset, int8_t yoffset, int8_t zoffset) {
  for (int i = 0; i < 6; i++) {
    oldVerts[i] = oldVerts[i] & 
  }


  /*for (size_t i = 0; i < vertBufferRowSize; i++) {
    for (size_t j = 0; j < numOfVertRows; j++)
      verts.push_back(oldVerts[i * numOfVertRows + j]);

    switch (cube.blockType) {
    case Grass:
      verts.push_back(0);
      break;
    case Snow:
      verts.push_back(1);
      break;
    case Dirt:
      verts.push_back(2);
      break;
    case Water:
      verts.push_back(3);
      break;
    }
  }

  for (size_t i = 0; i < numOfVertRows; i++) {
    verts[i * totalVertBufferRowSize] += xoffset;
    verts[i * totalVertBufferRowSize + 1] += yoffset;
    verts[i * totalVertBufferRowSize + 2] += zoffset;
  }
*/
  return verts;
}

std::vector<GLuint> cubeVerts(Cube &cube, int xoffset, int yoffset,
                              int zoffset) {
  if (!cube.isActive)
    return {};

  std::vector<int8_t> verts;
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

  glNamedBufferStorage(cube.vbo, sizeof(int8_t) * verts.size(), verts.data(),
                       GL_DYNAMIC_STORAGE_BIT);

  glVertexArrayVertexBuffer(cube.vao, 0, cube.vbo, 0,
                            sizeof(int8_t) * totalVertBufferRowSize);

  glEnableVertexArrayAttrib(cube.vao, 0);
  glEnableVertexArrayAttrib(cube.vao, 1);
  glEnableVertexArrayAttrib(cube.vao, 2);
  glEnableVertexArrayAttrib(cube.vao, 3);

  glVertexArrayAttribFormat(cube.vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribFormat(cube.vao, 1, 3, GL_FLOAT, GL_FALSE,
                            sizeof(int8_t) * 3);
  glVertexArrayAttribFormat(cube.vao, 2, 2, GL_FLOAT, GL_FALSE,
                            sizeof(int8_t) * 6);
  glVertexArrayAttribFormat(cube.vao, 3, 3, GL_FLOAT, GL_FALSE,
                            sizeof(int8_t) * 8);

  glVertexArrayAttribBinding(cube.vao, 0, 0);
  glVertexArrayAttribBinding(cube.vao, 1, 0);
  glVertexArrayAttribBinding(cube.vao, 2, 0);
  glVertexArrayAttribBinding(cube.vao, 3, 0);
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

  setInt(shader, "textureIndex", cube.diff);

  glBindVertexArray(cube.vao);
  glDrawArrays(GL_TRIANGLES, 0, cube.vertSize);
  glBindVertexArray(0);
}
