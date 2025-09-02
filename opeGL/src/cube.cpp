#include "cube.h"
#include "plane.h"
#include "shader.h"
#include "texture.h"
#include <glm/ext/matrix_transform.hpp>

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

Cube createCube(const char *diff, const char *spec, glm::vec3 pos,
                glm::vec3 rotation, float angle, float scale) {
  Cube cube;
  cube.pos = pos;
  cube.rotation = rotation;
  cube.angle = angle;
  cube.scale = scale;

  glCreateVertexArrays(1, &cube.vao);
  glCreateBuffers(1, &cube.vbo);

  glNamedBufferStorage(cube.vbo, sizeof(cubeVertices), cubeVertices,
                       GL_DYNAMIC_STORAGE_BIT);

  glVertexArrayVertexBuffer(cube.vao, 0, cube.vbo, 0, sizeof(GLfloat) * 8);

  glEnableVertexArrayAttrib(cube.vao, 0);
  glEnableVertexArrayAttrib(cube.vao, 1);
  glEnableVertexArrayAttrib(cube.vao, 2);

  glVertexArrayAttribFormat(cube.vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribFormat(cube.vao, 1, 3, GL_FLOAT, GL_FALSE,
                            sizeof(GLfloat) * 3);
  glVertexArrayAttribFormat(cube.vao, 2, 2, GL_FLOAT, GL_FALSE,
                            sizeof(GLfloat) * 6);

  glVertexArrayAttribBinding(cube.vao, 0, 0);
  glVertexArrayAttribBinding(cube.vao, 1, 0);
  glVertexArrayAttribBinding(cube.vao, 2, 0);

  cube.diff = loadTexture(diff);
  cube.spec = loadTexture(spec);

  return cube;
}

void drawCube(Cube &cube, GLuint shader) {
  setInt(shader, "tiling", 1);
  auto model = glm::mat4{1.0f};
  model = glm::translate(model, cube.pos);
  model = glm::rotate(model, glm::radians(cube.angle), cube.rotation);
  model = glm::scale(model, glm::vec3{cube.scale});
  setMat4(shader, "model", model);

  setInt(shader, "material.diffuse", 0);
  glBindTextureUnit(0, cube.diff);

  setInt(shader, "material.specular", 1);
  glBindTextureUnit(1, cube.spec);

  glBindVertexArray(cube.vao);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}
