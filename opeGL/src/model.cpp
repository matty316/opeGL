#include "model.h"
#include "glm/ext/matrix_transform.hpp"
#include "shader.h"

struct PerFrameData {
  glm::mat4 mvp;
  int isWireframe;
};

const GLsizeiptr kBufferSize = sizeof(PerFrameData);

Model createModel(glm::vec3 pos, float scale, const uint32_t *indices,
                  uint32_t indicesSizeBytes, const float *vertexData,
                  uint32_t verticesSizeBytes) {
  Model model;
  model.pos = pos;
  model.scale = scale;
  model.numIndices = (indicesSizeBytes / sizeof(uint32_t));

  glCreateBuffers(1, &model.perFrameDataBuffer);
  glNamedBufferStorage(model.perFrameDataBuffer, kBufferSize, nullptr,
                       GL_DYNAMIC_STORAGE_BIT);
  glBindBufferRange(GL_UNIFORM_BUFFER, 0, model.perFrameDataBuffer, 0,
                    kBufferSize);

  glEnable(GL_POLYGON_OFFSET_LINE);
  glPolygonOffset(-1.0f, -1.0f);

  glCreateVertexArrays(1, &model.vao);
  glCreateBuffers(1, &model.bufferIndices);
  glNamedBufferStorage(model.bufferIndices, indicesSizeBytes, indices, 0);
  glCreateBuffers(1, &model.bufferVertices);
  glNamedBufferStorage(model.bufferVertices, verticesSizeBytes, vertexData, 0);

  glVertexArrayElementBuffer(model.vao, model.bufferIndices);
  glVertexArrayVertexBuffer(model.vao, 0, model.bufferVertices, 0,
                            sizeof(glm::vec3));
  glEnableVertexArrayAttrib(model.vao, 0);
  glVertexArrayAttribFormat(model.vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribBinding(model.vao, 0, 0);
  return model;
}

void drawModel(Model model, GLuint shader, glm::mat4 v, glm::mat4 p) {
  use(shader);

  auto m = glm::mat4(1.0f);
  m = glm::translate(m, model.pos);
  m = glm::scale(m, glm::vec3(model.scale));
  PerFrameData perFrameData = {.mvp = p * v * m, .isWireframe = false};

  glBindVertexArray(model.vao);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glNamedBufferSubData(model.perFrameDataBuffer, 0, kBufferSize, &perFrameData);
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(model.numIndices),
                 GL_UNSIGNED_INT, nullptr);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  perFrameData.isWireframe = true;
  glNamedBufferSubData(model.perFrameDataBuffer, 0, kBufferSize, &perFrameData);
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(model.numIndices),
                 GL_UNSIGNED_INT, nullptr);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
