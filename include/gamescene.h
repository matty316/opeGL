#pragma once

#include "shader.h"
#include <common.h>
void createScene(Shader &shader);
void processMouse(GLFWwindow *window, double xposIn, double yposIn);
void processScroll(double yoffset);
void updateScene(int width, int height);
void processInput(GLFWwindow *window);
void renderScene(GLFWwindow *window, Shader &shader);
