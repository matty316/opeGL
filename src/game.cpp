#define STB_IMAGE_IMPLEMENTATION
#include "game.h"
#include "error.h"
#include "gamescene.h"
#include "stb_image.h"
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <cstdlib>
#include <print>

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  updateScene(width, height);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  processMouse(window, xposIn, yposIn);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  processScroll(yoffset);
}

void run() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

  GLFWwindow *window = glfwCreateWindow(800, 600, "OpeGL", nullptr, nullptr);
  if (window == nullptr) {
    std::println("Failed to create window");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::println("Failed to init glad");
    exit(EXIT_FAILURE);
  }

  enableReportGlErrors();

  glEnable(GL_DEPTH_TEST);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  stbi_set_flip_vertically_on_load(true);

  createScene();
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  while (!glfwWindowShouldClose(window)) {
    processInput(window);
    renderScene(window);
  }

  glfwTerminate();
}
