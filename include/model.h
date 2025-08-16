#pragma once

#include "mesh.h"
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Model {
  GLuint vao, meshData, perFrameDataBuffer;
  glm::vec3 position{0.0f};
  glm::vec3 rotation{1.0f};
  float rotationAngle = 0.0f;
  float scale = 0.5f;
};

Model createModel(const char *path, glm::vec3 pos, glm::vec3 rotation,
                  float rotationAngle, float scale);
void drawModel(const Model &model, GLuint shader);

