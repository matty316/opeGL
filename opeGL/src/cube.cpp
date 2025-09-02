#include "cube.h"
#include "glm/fwd.hpp"
#include "shader.h"
#include "texture.h"
#include <glm/ext/matrix_transform.hpp>
#include <print>
#include <vector>

GLuint vao, vbo, instanceVbo, diff, spec;

// clang-format off
GLfloat cubeVertices[] = {
    // Back face
    -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
     1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
     1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
     1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
    -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
    // Front face
    -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
     1.0f, -1.0f,  1.0f,  0.0f, 0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
     1.0f,  1.0f,  1.0f,  0.0f, 0.0f,  1.0f, 1.0f, 1.0f, // top-right
     1.0f,  1.0f,  1.0f,  0.0f, 0.0f,  1.0f, 1.0f, 1.0f, // top-right
    -1.0f,  1.0f,  1.0f,  0.0f, 0.0f,  1.0f, 0.0f, 1.0f, // top-left
    -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
    // Left face
    -1.0f,  1.0f,  1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f, // top-right
    -1.0f,  1.0f, -1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 1.0f, // top-left
    -1.0f, -1.0f, -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
    -1.0f, -1.0f, -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
    -1.0f, -1.0f,  1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
    -1.0f,  1.0f,  1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f, // top-right
    // Right face
     1.0f,  1.0f,  1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f, // top-left
     1.0f, -1.0f, -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
     1.0f,  1.0f, -1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 1.0f, // top-right         
     1.0f, -1.0f, -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
     1.0f,  1.0f,  1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f, // top-left
     1.0f, -1.0f,  1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
    // Bottom face
    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
     1.0f, -1.0f, -1.0f,  0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
     1.0f, -1.0f,  1.0f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
     1.0f, -1.0f,  1.0f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
    -1.0f, -1.0f,  1.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
    // Top face
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f, 0.0f, 0.0f, 1.0f, // top-left
     1.0f,  1.0f,  1.0f,  0.0f,  1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
     1.0f,  1.0f, -1.0f,  0.0f,  1.0f, 0.0f, 1.0f, 1.0f, // top-right     
     1.0f,  1.0f,  1.0f,  0.0f,  1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f, 0.0f, 0.0f, 1.0f, // top-left
    -1.0f,  1.0f,  1.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f  // bottom-left        
};
// clang-format on

bool buffersLoaded = false;
uint32_t instances = 0;

std::vector<Cube> cubes;
std::vector<glm::mat4> cubeMatrices;

void setupBuffers() {
  std::println("loading buffers");
  glCreateVertexArrays(1, &vao);
  glCreateBuffers(1, &vbo);

  glNamedBufferStorage(vbo, sizeof(cubeVertices), cubeVertices,
                       GL_DYNAMIC_STORAGE_BIT);
  glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(GLfloat) * 8);

  glEnableVertexArrayAttrib(vao, 0);
  glEnableVertexArrayAttrib(vao, 1);
  glEnableVertexArrayAttrib(vao, 2);

  glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3);
  glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6);

  glVertexArrayAttribBinding(vao, 0, 0);
  glVertexArrayAttribBinding(vao, 1, 0);
  glVertexArrayAttribBinding(vao, 2, 0);

  buffersLoaded = true;
}

void setupInstanceBuffer() {
  glBindVertexArray(vao);
  glCreateBuffers(1, &instanceVbo);
  glNamedBufferStorage(instanceVbo, instances * sizeof(glm::mat4), cubeMatrices.data(),
                       GL_DYNAMIC_STORAGE_BIT);
  glBindVertexBuffer(0, instanceVbo, 0, sizeof(glm::vec4) * 4);

  glEnableVertexArrayAttrib(vao, 3);
  glEnableVertexArrayAttrib(vao, 4);
  glEnableVertexArrayAttrib(vao, 5);
  glEnableVertexArrayAttrib(vao, 6);

  glVertexArrayAttribFormat(vao, 3, 4, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribFormat(vao, 4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4));
  glVertexArrayAttribFormat(vao, 5, 4, GL_FLOAT, GL_FALSE,
                            2 * sizeof(glm::vec4));
  glVertexArrayAttribFormat(vao, 6, 4, GL_FLOAT, GL_FALSE,
                            3 * sizeof(glm::vec4));

  glVertexArrayAttribBinding(vao, 3, 0);
  glVertexArrayAttribBinding(vao, 4, 0);
  glVertexArrayAttribBinding(vao, 5, 0);
  glVertexArrayAttribBinding(vao, 6, 0);

  glVertexArrayBindingDivisor(vao, 3, 1);
  glVertexArrayBindingDivisor(vao, 4, 1);
  glVertexArrayBindingDivisor(vao, 5, 1);
  glVertexArrayBindingDivisor(vao, 6, 1);

}

void createCube(const char *diffPath, const char *specPath, glm::vec3 pos,
                glm::vec3 rotation, float angle, float scale) {
  Cube cube;
  cube.pos = pos;
  cube.rotation = rotation;
  cube.angle = angle;
  cube.scale = scale;

  if (!buffersLoaded)
    setupBuffers();

  instances++;

  diff = loadTexture(diffPath);
  spec = loadTexture(specPath);

  cubeMatrices.push_back(cubeModelMatrix(cube));
  cubes.push_back(cube);
}

glm::mat4 cubeModelMatrix(Cube &cube) {
  auto model = glm::mat4{1.0f};
  model = glm::translate(model, cube.pos);
  model = glm::rotate(model, glm::radians(cube.angle), cube.rotation);
  model = glm::scale(model, glm::vec3{cube.scale});
  return model;
}

void drawCubes(GLuint shader) {
  use(shader);
  setInt(shader, "tiling", 1);
  setInt(shader, "material.diffuse", 0);
  glBindTextureUnit(0, diff);

  setInt(shader, "material.specular", 1);
  glBindTextureUnit(1, spec);

  glBindVertexArray(vao);
  glDrawArraysInstanced(GL_TRIANGLES, 0, 36, instances);
  glBindVertexArray(0);
}
