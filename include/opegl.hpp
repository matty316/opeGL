#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <string>
#include <vector>

#include "camera.hpp"
#include "level.hpp"
#include "light.hpp"
#include "model.hpp"
#include "quad.hpp"
#include "texture.hpp"

class OpeGL {
public:
  OpeGL();
  void run();
  void addQuad(glm::vec3 position, float angle, glm::vec3 rotation, float scale,
               size_t texture);
  size_t addTexture(const std::string &filename);
  void setPlayerPos(glm::vec2 pos);
  void addPointLight(PointLight &light);
  void loadLevel(std::string path, uint32_t wallTexture, uint32_t floorTexture,
                 uint32_t ceilingTexture, size_t maxHeight = 2);
  void addModel(std::string modelPath, glm::vec3 pos, float angle,
                glm::vec3 rotation, float scale);

private:
  GLFWwindow *window;
  GLuint vao, vbo, ebo, perInstanceDataBuffer;
  OpeCamera camera{{0.0f, 0.0f, 3.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};
  struct MouseState {
    glm::vec2 pos{0.0f};
    bool pressed = false;
  } mouseState;

  std::vector<PointLight> pointLights;
  std::vector<OpeModel> models;

  OpeLevel *currentLevel = nullptr;

  double timeStamp = glfwGetTime();
  double deltaTime = 0.0f;
  bool fullscreen = false;
  bool debug = true;
  OpeQuad quad;
  OpeTexture textures;

  void init();
  void createBuffers();
  void mainLoop();
  void cleanup();
  void update();
  static void framebuffer_size_callback(GLFWwindow *window, int width,
                                        int height);
  static void mouse_callback(GLFWwindow *window, double x, double y);
  static void key_callback(GLFWwindow *window, int key, int scancode,
                           int action, int mods);
  void processInput(GLFWwindow *window);
  static void APIENTRY glDebugOutput(GLenum source, GLenum type,
                                     unsigned int id, GLenum severity,
                                     GLsizei length, const char *message,
                                     const void *userParam);
};
