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

GLfloat backUVs[] = {
      0.0f, 0.0f, // Bottom-left
      1.0f, 1.0f, // top-right
      1.0f, 0.0f, // bottom-right
      1.0f, 1.0f, // top-right
      0.0f, 0.0f, // bottom-left
      0.0f, 1.0f, // top-left
};
GLfloat frontUVs[] = {
      0.0f, 0.0f, // bottom-left
      1.0f, 0.0f, // bottom-right
      1.0f, 1.0f, // top-right
      1.0f, 1.0f, // top-right
      0.0f, 1.0f, // top-left
      0.0f, 0.0f, // bottom-left
};
GLfloat leftUVs[] = {
      1.0f, 0.0f, // top-right
      1.0f, 1.0f, // top-left
      0.0f, 1.0f, // bottom-left
      0.0f, 1.0f, // bottom-left
      0.0f, 0.0f, // bottom-right
      1.0f, 0.0f, // top-right
};
GLfloat rightUVs[] = {
      1.0f, 0.0f, // top-left
      0.0f, 1.0f, // bottom-right
      1.0f, 1.0f, // top-right
      0.0f, 1.0f, // bottom-right
      1.0f, 0.0f, // top-left
      0.0f, 0.0f, // bottom-left
};
GLfloat bottomUVs[] = {
      0.0f, 1.0f, // top-right
      1.0f, 1.0f, // top-left
      1.0f, 0.0f, // bottom-left
      1.0f, 0.0f, // bottom-left
      0.0f, 0.0f, // bottom-right
      0.0f, 1.0f, // top-right
};
GLfloat topUVs[] = {
      0.0f, 1.0f, // top-left
      1.0f, 0.0f, // bottom-right
      1.0f, 1.0f, // top-right
      1.0f, 0.0f, // bottom-right
      0.0f, 1.0f, // top-left
      0.0f, 0.0f  // bottom-left
};
// clang-format on


std::vector<GLfloat> vertsWithOffset(GLfloat oldVerts[36], GLfloat uvs[12], int xoffset, int yoffset, int zoffset) {
  const size_t vertBufferRowSize = 6, numOfVertRows = 6, vertBufferRowSizeWithUVs = 8;
  std::vector<GLfloat> verts; 
  
  for (size_t i = 0; i < vertBufferRowSize; i++) {
    for (size_t j = 0; j < numOfVertRows; j++)
      verts.push_back(oldVerts[i * numOfVertRows +  j]);

    verts.push_back(uvs[i * 2]);
    verts.push_back(uvs[i * 2 + 1]);
  }

  for (size_t i = 0; i < numOfVertRows; i++) {
    verts[i * vertBufferRowSizeWithUVs] += xoffset;
    verts[i * vertBufferRowSizeWithUVs + 1] += yoffset;
    verts[i * vertBufferRowSizeWithUVs + 2] += zoffset;
  }

  return verts;
}

std::vector<GLfloat> cubeVerts(Cube &cube, int xoffset, int yoffset, int zoffset) {
  if (!cube.isActive)
    return {};

  std::vector<GLfloat> verts;
  size_t vertSize = 0; 
  const size_t vertsPerRow = 6;
  if (cube.back) {
    auto newVerts = vertsWithOffset(cubeBackFace, backUVs, xoffset, yoffset, zoffset); 
    for (auto &vert : newVerts)
      verts.push_back(vert);
    vertSize += vertsPerRow;
  }
  if (cube.front) {
    auto newVerts = vertsWithOffset(cubeFrontFace, frontUVs, xoffset, yoffset, zoffset); 
    for (auto &vert : newVerts)
      verts.push_back(vert);
    vertSize += vertsPerRow;
  }
  if (cube.left) {
    auto newVerts = vertsWithOffset(cubeLeftFace, leftUVs, xoffset, yoffset, zoffset); 
    for (auto &vert : newVerts)
      verts.push_back(vert);
    vertSize += vertsPerRow;
  }
  if (cube.right) {
    auto newVerts = vertsWithOffset(cubeRightFace, rightUVs, xoffset, yoffset, zoffset); 
    for (auto &vert : newVerts)
      verts.push_back(vert);
    vertSize += vertsPerRow;
  }
  if (cube.bottom) {
    auto newVerts = vertsWithOffset(cubeBottomFace, bottomUVs, xoffset, yoffset, zoffset); 
    for (auto &vert : newVerts)
      verts.push_back(vert);
    vertSize += vertsPerRow;
  }
  if (cube.top) { 
    auto newVerts = vertsWithOffset(cubeTopFace, topUVs, xoffset, yoffset, zoffset); 
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

  glNamedBufferStorage(cube.vbo, sizeof(GLfloat) * verts.size(), verts.data(), GL_DYNAMIC_STORAGE_BIT);

  glVertexArrayVertexBuffer(cube.vao, 0, cube.vbo, 0, sizeof(GLfloat) * 8);

  glEnableVertexArrayAttrib(cube.vao, 0);
  glEnableVertexArrayAttrib(cube.vao, 1);
  glEnableVertexArrayAttrib(cube.vao, 2);

  glVertexArrayAttribFormat(cube.vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribFormat(cube.vao, 1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3);
  glVertexArrayAttribFormat(cube.vao, 2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6);

  glVertexArrayAttribBinding(cube.vao, 0, 0);
  glVertexArrayAttribBinding(cube.vao, 1, 0);
  glVertexArrayAttribBinding(cube.vao, 2, 0);
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
