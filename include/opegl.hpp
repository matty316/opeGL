#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include "camera.hpp"

class OpeGL {
public:
  void run();

private:
  GLFWwindow *window;
  GLuint vao, vbo, ebo;
  OpeCamera camera{{0.0f, 0.0f, 3.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};
  struct MouseState {
    glm::vec2 pos{0.0f};
    bool pressed = false;
  } mouseState;

  double timeStamp = glfwGetTime();
  double deltaTime = 0.0f;
  bool fullscreen = true;

  void init();
  void mainLoop();
  void cleanup();
  void update();
  static void framebuffer_size_callback(GLFWwindow *window, int width,
                                        int height);
  static void mouse_callback(GLFWwindow *window, double x, double y);
  static void key_callback(GLFWwindow *window, int key, int scancode,
                           int action, int mods);
  void processInput(GLFWwindow *window);
};
