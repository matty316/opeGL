#pragma once

#include <common.h>
void createScene();
void processMouse(GLFWwindow *window, double xposIn, double yposIn);
void processScroll(double yoffset);
void updateScene(int width, int height);
void processInput(GLFWwindow *window);
void renderScene(GLFWwindow *window);
