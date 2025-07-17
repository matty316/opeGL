#pragma once

#include "shader.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoords;
};

struct Texture {
  GLuint id;
  std::string type;
  std::string path;
};

struct Mesh {
  GLuint VAO, VBO, EBO;
  std::vector<Texture> textures;
  std::vector<unsigned int> indices;
  std::vector<Vertex> vertices;
};

Mesh createMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
                std::vector<Texture> textures);
void drawMesh(Mesh &mesh, GLuint shader);
