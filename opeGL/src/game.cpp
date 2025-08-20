#include "camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "error.h"
#include "game.h"
#include "gamescene.h"
#include "stb_image.h"
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <cstdlib>
#include <print>

GLFWwindow *window;
CameraMovement movement;

struct MouseState {
  glm::vec2 pos = glm::vec2(0.0f);
  bool pressedLeft = false;
} mouseState;

void mouse_callback(GLFWwindow *window, double x, double y) {
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  mouseState.pos.x = static_cast<float>(x / width);
  mouseState.pos.y = static_cast<float>(y / height);
}

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT)
    mouseState.pressedLeft = action == GLFW_PRESS;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  const bool pressed = action != GLFW_RELEASE;
  if (key == GLFW_KEY_ESCAPE && pressed)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  if (key == GLFW_KEY_W)
    movement.forward = pressed;
  if (key == GLFW_KEY_S)
    movement.backward = pressed;
  if (key == GLFW_KEY_A)
    movement.left = pressed;
  if (key == GLFW_KEY_D)
    movement.right = pressed;
}

void start() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

  window =
      glfwCreateWindow(800, 600, "OpeGL", glfwGetPrimaryMonitor(), nullptr);
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
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetKeyCallback(window, key_callback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  stbi_set_flip_vertically_on_load(true);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  createCamera();
}

void run() {
  double timeStamp = glfwGetTime();
  float deltaTime = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    updateCamera(movement, deltaTime, mouseState.pos, mouseState.pressedLeft);

    const double newTimeStamp = glfwGetTime();
    deltaTime = static_cast<float>(newTimeStamp - timeStamp);
    timeStamp = newTimeStamp;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    updateScene(width, height);
    renderScene(window);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}
