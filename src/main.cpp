
#include "error.h"
#include "gamescene.h"
#include "shader.h"
#include "model.h"
#include <iostream>
#include <stb_image.h>

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

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

  GLFWwindow *window = glfwCreateWindow(800, 600, "OpeGL", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to init glad" << std::endl;
    return -1;
  }

//  enableReportGlErrors();

  glEnable(GL_DEPTH_TEST);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  stbi_set_flip_vertically_on_load(true);

    Shader shader{"../resources/shader.vert", "../resources/shader.frag"};
    Cube cube{shader, "../resources/container2.png",
              "../resources/container2_specular.png"};
    createScene(shader);
  while (!glfwWindowShouldClose(window)) {
    processInput(window);
    renderScene(window, shader, cube);
  }

  glfwTerminate();
  return 0;
}
