#include "opegl.hpp"
#include "GLFW/glfw3.h"
#include "constants.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/trigonometric.hpp"
#include "shader.hpp"
#include "vertex.hpp"
#include <cstddef>
#include <cstdint>
#include <stdexcept>

OpeGL::OpeGL() { init(); }

void OpeGL::run() {
  createBuffers();
  mainLoop();
  cleanup();
}

void OpeGL::mainLoop() {
  OpeShader shader;
  shader.use();
  shader.setTextures(textures.textureCount());
  while (!glfwWindowShouldClose(window)) {
    update();
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    processInput(window);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();

    auto aspect = static_cast<float>(width) / static_cast<float>(height);
    auto projection =
        glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
    shader.setMat4("projection", projection);

    shader.setMat4("view", camera.getView());

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, modelMatrixBuffer);

    glBindVertexArray(vao);
    glDrawElementsInstanced(GL_TRIANGLES, quadIndices.size(), GL_UNSIGNED_INT,
                            0, quad.getInstanceCount());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  shader.cleanup();
}

void OpeGL::init() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window =
      glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "opegl",
                       fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
  if (window == nullptr)
    throw std::runtime_error("failed to create window");

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    throw std::runtime_error("failed to initialize GLAD");
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetKeyCallback(window, key_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void OpeGL::createBuffers() {
  glCreateBuffers(1, &vbo);
  glNamedBufferStorage(vbo, sizeof(Vertex) * quadVertices.size(),
                       quadVertices.data(), GL_DYNAMIC_STORAGE_BIT);

  glCreateBuffers(1, &ebo);
  glNamedBufferStorage(ebo, sizeof(uint32_t) * quadIndices.size(),
                       quadIndices.data(), GL_DYNAMIC_STORAGE_BIT);

  glCreateVertexArrays(1, &vao);

  glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));
  glVertexArrayElementBuffer(vao, ebo);

  glEnableVertexArrayAttrib(vao, 0);
  glEnableVertexArrayAttrib(vao, 1);

  glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE,
                            offsetof(Vertex, pos));
  glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE,
                            offsetof(Vertex, texCoord));

  glVertexArrayAttribBinding(vao, 0, 0);
  glVertexArrayAttribBinding(vao, 1, 0);

  glCreateBuffers(1, &modelMatrixBuffer);
  glNamedBufferStorage(
      modelMatrixBuffer, sizeof(PerInstanceData) * quad.getInstanceCount(),
      quad.getPerInstanceData().data(), GL_DYNAMIC_STORAGE_BIT);

  for (size_t i = 0; i < textures.textureCount(); i++) {
    glBindTextureUnit(i, textures.textureIdAtCount(i));
  }
}

void OpeGL::cleanup() {
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
  glfwTerminate();
}

void OpeGL::framebuffer_size_callback(GLFWwindow *window, int width,
                                      int height) {
  glViewport(0, 0, width, height);
}

void OpeGL::processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void OpeGL::key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  auto app = reinterpret_cast<OpeGL *>(glfwGetWindowUserPointer(window));
  const bool press = action != GLFW_RELEASE;
  if (key == GLFW_KEY_ESCAPE)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  if (key == GLFW_KEY_W)
    app->camera.movement.forward = press;
  if (key == GLFW_KEY_S)
    app->camera.movement.backward = press;
  if (key == GLFW_KEY_A)
    app->camera.movement.left = press;
  if (key == GLFW_KEY_D)
    app->camera.movement.right = press;
  if (key == GLFW_KEY_LEFT_SHIFT)
    app->camera.movement.fast = press;
}

void OpeGL::mouse_callback(GLFWwindow *window, double x, double y) {
  auto app = reinterpret_cast<OpeGL *>(glfwGetWindowUserPointer(window));
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);

  app->mouseState.pos.x = static_cast<float>(x / width);
  app->mouseState.pos.y = static_cast<float>(y / height);
}

void OpeGL::update() {
  const double newTimeStamp = glfwGetTime();
  deltaTime = newTimeStamp - timeStamp;
  timeStamp = newTimeStamp;
  camera.update(deltaTime, mouseState.pos);
}

void OpeGL::addQuad(glm::vec3 position, float angle, glm::vec3 rotation,
                    float scale, size_t texture) {
  quad.addQuad(position, angle, rotation, scale,
               static_cast<uint32_t>(texture));
}

size_t OpeGL::addTexture(const std::string &filename) {
  return textures.loadTexture(filename);
}
