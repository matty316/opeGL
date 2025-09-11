#include "camera.h"
#include "cube.h"
#include "frameCounter.h"
#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "game.h"
#include "gamescene.h"
#include "stb_image.h"
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <cstdlib>
#include <iostream>
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
  if (key == GLFW_KEY_LEFT_SHIFT)
    movement.fastSpeed = pressed;
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id,
                            GLenum severity, GLsizei length,
                            const char *message, const void *userParam) {
  // ignore non-significant error/warning codes
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    return;

  std::cout << "---------------" << std::endl;
  std::cout << "Debug message (" << id << "): " << message << std::endl;

  switch (source) {
  case GL_DEBUG_SOURCE_API:
    std::cout << "Source: API";
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    std::cout << "Source: Window System";
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    std::cout << "Source: Shader Compiler";
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    std::cout << "Source: Third Party";
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    std::cout << "Source: Application";
    break;
  case GL_DEBUG_SOURCE_OTHER:
    std::cout << "Source: Other";
    break;
  }
  std::cout << std::endl;

  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    std::cout << "Type: Error";
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    std::cout << "Type: Deprecated Behaviour";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    std::cout << "Type: Undefined Behaviour";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    std::cout << "Type: Portability";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    std::cout << "Type: Performance";
    break;
  case GL_DEBUG_TYPE_MARKER:
    std::cout << "Type: Marker";
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    std::cout << "Type: Push Group";
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    std::cout << "Type: Pop Group";
    break;
  case GL_DEBUG_TYPE_OTHER:
    std::cout << "Type: Other";
    break;
  }
  std::cout << std::endl;

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    std::cout << "Severity: high";
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    std::cout << "Severity: medium";
    break;
  case GL_DEBUG_SEVERITY_LOW:
    std::cout << "Severity: low";
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    std::cout << "Severity: notification";
    break;
  }
  std::cout << std::endl;
  std::cout << std::endl;
}

void start(bool debug, bool fullscreen, bool vSync) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  if (debug)
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

  window =
      glfwCreateWindow(800, 600, "OpeGL", fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
  if (window == nullptr) {
    std::println("Failed to create window");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetKeyCallback(window, key_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::println("Failed to init glad");
    exit(EXIT_FAILURE);
  }

  if (!vSync)
    glfwSwapInterval(0);

  if (debug) {
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
      // initialize debug output
      glEnable(GL_DEBUG_OUTPUT);
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      glDebugMessageCallback(glDebugOutput, nullptr);
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
                            nullptr, GL_TRUE);
    }
  }

  glEnable(GL_DEPTH_TEST);
  stbi_set_flip_vertically_on_load(true);
  glEnable(GL_CULL_FACE);
}

void run(CameraType cameraType, glm::vec3 cameraPosition, bool wireframe) {
  createCamera(cameraType, cameraPosition);
  double timeStamp = glfwGetTime();
  float deltaTime = 0.0f;

  if (wireframe)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    updateCamera(movement, deltaTime, mouseState.pos, mouseState.pressedLeft);

    const double newTimeStamp = glfwGetTime();
    deltaTime = static_cast<float>(newTimeStamp - timeStamp);
    timeStamp = newTimeStamp;

    tick(deltaTime);

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
