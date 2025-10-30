#pragma once

#include "vertex.hpp"

#include <cstdint>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

class OpeModel {
public:
  OpeModel(std::string modelPath, glm::vec3 pos, float angle,
           glm::vec3 rotation, float scale)
      : modelPath(modelPath), pos(pos), angle(angle), rotation(rotation),
        scale(scale) {}
  void loadModel();
  glm::mat4 modelMatrix();
  uint32_t getIndicesSize();
  void setDiffuse(size_t diffuse) { textureId = diffuse; }
  size_t getTextureId() { return textureId; }
  std::string getDiffusePath() { return diffusePath; }
  void draw(uint32_t offset);

private:
  std::string modelPath;
  std::string diffusePath;
  size_t textureId;
  glm::vec3 pos;
  float angle;
  glm::vec3 rotation;
  float scale;
  GLuint vao, vbo, ebo;

  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
};
