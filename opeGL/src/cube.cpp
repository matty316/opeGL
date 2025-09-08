#include "cube.h"
#include "glm/fwd.hpp"
#include "shader.h"
#include <cstddef>
#include <glm/ext/matrix_transform.hpp>
#include <vector>

// clang-format off

GLfloat cubeBackFace[] = {
    -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f, // Bottom-left
     1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f, // top-right
     1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f, // bottom-right         
     1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f, // top-right
    -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f, // bottom-left
    -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f, // top-left
};

GLfloat cubeFrontFace[] = {
    -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,  1.0f, // bottom-left
     1.0f, -1.0f,  1.0f,  0.0f, 0.0f,  1.0f, // bottom-right
     1.0f,  1.0f,  1.0f,  0.0f, 0.0f,  1.0f, // top-right
     1.0f,  1.0f,  1.0f,  0.0f, 0.0f,  1.0f, // top-right
    -1.0f,  1.0f,  1.0f,  0.0f, 0.0f,  1.0f, // top-left
    -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,  1.0f, // bottom-left
};

GLfloat cubeLeftFace[] = {
    -1.0f,  1.0f,  1.0f, -1.0f, 0.0f,  0.0f, // top-right
    -1.0f,  1.0f, -1.0f, -1.0f, 0.0f,  0.0f, // top-left
    -1.0f, -1.0f, -1.0f, -1.0f, 0.0f,  0.0f, // bottom-left
    -1.0f, -1.0f, -1.0f, -1.0f, 0.0f,  0.0f, // bottom-left
    -1.0f, -1.0f,  1.0f, -1.0f, 0.0f,  0.0f, // bottom-right
    -1.0f,  1.0f,  1.0f, -1.0f, 0.0f,  0.0f, // top-right
};

GLfloat cubeRightFace[] = {
     1.0f,  1.0f,  1.0f,  1.0f, 0.0f,  0.0f, // top-left
     1.0f, -1.0f, -1.0f,  1.0f, 0.0f,  0.0f, // bottom-right
     1.0f,  1.0f, -1.0f,  1.0f, 0.0f,  0.0f, // top-right         
     1.0f, -1.0f, -1.0f,  1.0f, 0.0f,  0.0f, // bottom-right
     1.0f,  1.0f,  1.0f,  1.0f, 0.0f,  0.0f, // top-left
     1.0f, -1.0f,  1.0f,  1.0f, 0.0f,  0.0f, // bottom-left
};

GLfloat cubeBottomFace[] = {
    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f, 0.0f, // top-right
     1.0f, -1.0f, -1.0f,  0.0f, -1.0f, 0.0f, // top-left
     1.0f, -1.0f,  1.0f,  0.0f, -1.0f, 0.0f, // bottom-left
     1.0f, -1.0f,  1.0f,  0.0f, -1.0f, 0.0f, // bottom-left
    -1.0f, -1.0f,  1.0f,  0.0f, -1.0f, 0.0f, // bottom-right
    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f, 0.0f, // top-right 
};

GLfloat cubeTopFace[] = {
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f, 0.0f, // top-left
     1.0f,  1.0f,  1.0f,  0.0f,  1.0f, 0.0f, // bottom-right
     1.0f,  1.0f, -1.0f,  0.0f,  1.0f, 0.0f, // top-right     
     1.0f,  1.0f,  1.0f,  0.0f,  1.0f, 0.0f, // bottom-right
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f, 0.0f, // top-left
    -1.0f,  1.0f,  1.0f,  0.0f,  1.0f, 0.0f, // bottom-left
};
// clang-format on

std::vector<GLfloat> vertsWithOffset(GLfloat oldVerts[36], int xoffset, int yoffset, int zoffset) {
  std::vector<GLfloat> verts; 

  for (size_t i = 0; i < 36; i++)
    verts.push_back(oldVerts[i]);

  for (size_t i = 0; i < 6; i++) {
    verts[i * 6] += xoffset;
    verts[(i * 6) + 1] = yoffset;
    verts[(i * 6) + 2] = zoffset;
  }

  return verts;
}

std::vector<GLfloat> cubeVerts(Cube &cube, int xoffset, int yoffset, int zoffset) {
  std::vector<GLfloat> verts;
  size_t vertSize = 0;
  if (cube.back) {
    auto newVerts = vertsWithOffset(cubeBackFace, xoffset, yoffset, zoffset);
    for (size_t i = 0; i < 36; i++)
      verts.push_back(newVerts[i]);
    vertSize += 6;
  }
  if (cube.front) {
    auto newVerts = vertsWithOffset(cubeFrontFace, xoffset, yoffset, zoffset);
    for (size_t i = 0; i < 36; i++)
      verts.push_back(newVerts[i]);
    vertSize += 6;
  }
  if (cube.left) {
    auto newVerts = vertsWithOffset(cubeLeftFace, xoffset, yoffset, zoffset);
    for (size_t i = 0; i < 36; i++)
      verts.push_back(newVerts[i]);
    vertSize += 6;
  }
  if (cube.right) {
    auto newVerts = vertsWithOffset(cubeRightFace, xoffset, yoffset, zoffset);
    for (size_t i = 0; i < 36; i++)
      verts.push_back(newVerts[i]);
    vertSize += 6;
  }
  if (cube.bottom) {
    auto newVerts = vertsWithOffset(cubeBottomFace, xoffset, yoffset, zoffset);
    for (size_t i = 0; i < 36; i++)
      verts.push_back(newVerts[i]);
    vertSize += 6;
  }
  if (cube.top) { 
    auto newVerts = vertsWithOffset(cubeTopFace, xoffset, yoffset, zoffset);
    for (size_t i = 0; i < 36; i++)
      verts.push_back(newVerts[i]);
    vertSize += 6;
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

  glNamedBufferStorage(cube.vbo, sizeof(GLfloat) * verts.size(), verts.data(), GL_DYNAMIC_STORAGE_BIT);

  glVertexArrayVertexBuffer(cube.vao, 0, cube.vbo, 0, sizeof(GLfloat) * 6);

  glEnableVertexArrayAttrib(cube.vao, 0);
  glEnableVertexArrayAttrib(cube.vao, 1);

  glVertexArrayAttribFormat(cube.vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribFormat(cube.vao, 1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3);

  glVertexArrayAttribBinding(cube.vao, 0, 0);
  glVertexArrayAttribBinding(cube.vao, 1, 0);
}

Cube createCube(size_t diff, size_t spec, glm::vec3 pos, glm::vec3 rotation,
                float angle, float scale, BlockType blockType, bool deferBuffers) {
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
  auto model = glm::mat4{1.0f};
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
