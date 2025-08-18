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

GLFWwindow *window;

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  processMouse(window, xposIn, yposIn);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  processScroll(yoffset);
}

void start() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

  window = glfwCreateWindow(800, 600, "OpeGL", glfwGetPrimaryMonitor(), nullptr);
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
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  stbi_set_flip_vertically_on_load(true);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
}

void run() {
  while (!glfwWindowShouldClose(window)) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    updateScene(width, height);
    processInput(window);
    renderScene(window);
  }

  glfwTerminate();
}
