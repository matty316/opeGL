#pragma once

// clang-format off
#include "chunk.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "shader.h"
// clang-format on

void createScene();
void processMouse(GLFWwindow *window, double xposIn, double yposIn);
void processScroll(double yoffset);
void updateScene(int width, int height);
void processInput(GLFWwindow *window);
void renderScene(GLFWwindow *window);
void addModel(const char *path, glm::vec3 pos, glm::vec3 rotation, float angle,
              float scale);
void addPlane(GLuint64 diff, glm::vec3 pos, glm::vec3 rotation, float angle,
              float scale, int tiling = 16);
void addCube(size_t diff, size_t spec, glm::vec3 pos, glm::vec3 rotation,
             float angle, float scale);
void addChunk(size_t diff, size_t spec, glm::vec3 pos, glm::vec3 rotation,
              float angle, float scale, ChunkType type = Sphere,
              size_t chunkSize = 64);
