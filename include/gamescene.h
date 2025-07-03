#pragma once

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "shader.h"
// clang-format on

void createScene(Shader &shader);
void processMouse(GLFWwindow *window, double xposIn, double yposIn);
void processScroll(double yoffset);
void updateScene(int width, int height);
void processInput(GLFWwindow *window);
void renderScene(GLFWwindow *window, Shader &shader);
