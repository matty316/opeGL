#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

struct Model {
  GLuint vao, bufferIndices, bufferVertices, perFrameDataBuffer;
  uint32_t numIndices;
  glm::vec3 pos;
  float scale;
};

Model createModel(glm::vec3 pos, float scale, const uint32_t* indices, uint32_t indicesSizeBytes, const float* vertexData, uint32_t verticesSizeBytes);
void drawModel(Model model, GLuint shader, glm::mat4 v, glm::mat4 p);
