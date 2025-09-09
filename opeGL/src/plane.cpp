#include "plane.h"
#include "glad/glad.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include "shader.h"
#include "stb_image.h"
#include "texture.h"

float vertices[32] = {
    0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
    0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom left
    -0.5f, 0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f  // top left
};
GLuint indices[6] = {
    // note that we start from 0!
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

Plane createPlane(GLuint64 diff, glm::vec3 pos, glm::vec3 rotation, float angle,
                  float scale, int tiling) {
  Plane plane;
  glGenVertexArrays(1, &plane.vao);
  glGenBuffers(1, &plane.vbo);
  glGenBuffers(1, &plane.ebo);

  glBindVertexArray(plane.vao);

  glBindBuffer(GL_ARRAY_BUFFER, plane.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane.ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  plane.diff = diff;

  plane.pos = pos;
  plane.angle = angle;
  plane.rotation = rotation;
  plane.scale = scale;
  plane.tiling = tiling;
  return plane;
}

void drawPlane(Plane plane, GLuint shader) {
  setInt(shader, "tiling", plane.tiling);
  auto model = glm::mat4{1.0f};
  model = glm::translate(model, plane.pos);
  model = glm::rotate(model, glm::radians(plane.angle), plane.rotation);
  model = glm::scale(model, glm::vec3{plane.scale});
  setMat4(shader, "model", model);

  setInt(shader, "textureIndex", plane.diff);

  glBindVertexArray(plane.vao);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
