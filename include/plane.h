#pragma once

#include "glm/glm.hpp"
#include "shader.h"

struct Plane {
public:
  Plane(const char *diffusePath, const char *specularPath);
  void draw(Shader &shader);

private:
  unsigned int VAO, VBO, EBO, diffuse, specular;
  glm::vec3 position, rotation;
  float rotationAngle, scale;
  float vertices[20] = {
      0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 0.0f, 1.0f  // top left
  };
  unsigned int indices[6] = {
      // note that we start from 0!
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };

  unsigned int loadTexture(const char *path);
};
