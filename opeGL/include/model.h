#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoord;
};

struct Texture {
  uint32_t id;
  std::string type;
};

struct Mesh {
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  std::vector<Texture> texture;
  GLuint vao, vbo, ebo;
};

struct Model {
  GLuint vao, meshData, perFrameDataBuffer;
  glm::vec3 position{0.0f};
  glm::vec3 rotation{1.0f};
  float rotationAngle = 0.0f;
  float scale = 0.5f;
  int indicesSize = 0;
};

Mesh createMesh();
void drawMesh(const Mesh& mesh);
Model createModel(const char *path, glm::vec3 pos, glm::vec3 rotation,
                  float rotationAngle, float scale);
void drawModel(const Model &model, GLuint shader, glm::mat4 v, glm::mat4 p);
