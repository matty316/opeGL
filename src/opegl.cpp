#include "opegl.hpp"
#include "GLFW/glfw3.h"
#include "constants.hpp"
#include "glm/common.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/trigonometric.hpp"
#include "level.hpp"
#include "light.hpp"
#include "shader.hpp"
#include "vertex.hpp"
#include <cfloat>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <print>
#include <stdexcept>
#include <vector>

OpeGL::OpeGL() { init(); }

void OpeGL::run() {
  createBuffers();
  mainLoop();
  cleanup();
}

void OpeGL::mainLoop() {
  OpeShader shader;
  OpeShader modelShader(true);
  shader.use();
  shader.setTextures(textures.textureCount());
  modelShader.use();
  modelShader.setTextures(textures.textureCount());

  for (size_t i = 0; i < pointLights.size(); i++) {
    shader.use();
    shader.setPointLight(pointLights[i], i);
    modelShader.use();
    modelShader.setPointLight(pointLights[i], i);
  }

  while (!glfwWindowShouldClose(window)) {
    processInput(window);
    update();
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();

    auto aspect = static_cast<float>(width) / static_cast<float>(height);
    auto projection =
        glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
    shader.setMat4("projection", projection);

    shader.setMat4("view", camera.getView());
    shader.setVec3("viewPos", camera.getPosition());

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, perInstanceDataBuffer);

    glBindVertexArray(vao);
    glDrawElementsInstanced(GL_TRIANGLES, quadIndices.size(), GL_UNSIGNED_INT,
                            0, quad.getInstanceCount());

    modelShader.use();
    modelShader.setMat4("projection", projection);

    modelShader.setMat4("view", camera.getView());
    modelShader.setVec3("viewPos", camera.getPosition());

    for (auto &model : models) {
      modelShader.setMat4("modelMatrix", model.modelMatrix());
      modelShader.setUInt("textureIndx",
                          static_cast<int>(model.getTextureId()));
      model.draw(quad.getInstanceCount() + 1);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  shader.cleanup();
  modelShader.cleanup();
}

void OpeGL::init() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  if (debug)
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
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
  glEnable(GL_CULL_FACE);
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
  glEnableVertexArrayAttrib(vao, 2);

  glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE,
                            offsetof(Vertex, pos));
  glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE,
                            offsetof(Vertex, texCoord));
  glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, GL_FALSE,
                            offsetof(Vertex, normal));

  glVertexArrayAttribBinding(vao, 0, 0);
  glVertexArrayAttribBinding(vao, 1, 0);
  glVertexArrayAttribBinding(vao, 2, 0);

  glCreateBuffers(1, &perInstanceDataBuffer);

  glNamedBufferStorage(
      perInstanceDataBuffer,
      sizeof(PerInstanceData) * quad.getPerInstanceData().size(),
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
  if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
    if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
      GLFWgamepadstate state;
      if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
        float left_stick_x = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
        float left_stick_y = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
        float right_stick_x = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
        float right_stick_y = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];

        camera.movement.forward = left_stick_y < -0.5f;
        camera.movement.backward = left_stick_y > 0.5f;
        camera.movement.left = left_stick_x < -0.5f;
        camera.movement.right = left_stick_x > 0.5f;

        camera.updateRightAxes(deltaTime, right_stick_x, right_stick_y);
      }
    } else {
      int count;
      const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);

      if (count >= 4) {
        camera.movement.forward = axes[1] < -0.5f;
        camera.movement.backward = axes[1] > 0.5f;
        camera.movement.left = axes[0] < -0.5f;
        camera.movement.right = axes[0] > 0.5f;

        camera.updateRightAxes(deltaTime, axes[3], axes[4]);
      }
    }
  }
}

void OpeGL::key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  auto app = reinterpret_cast<OpeGL *>(glfwGetWindowUserPointer(window));
  const bool press = action != GLFW_RELEASE;

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
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
  if (checkCollision()) {
    std::println("collided");
  }
  camera.update(deltaTime, mouseState.pos, false);
}

void OpeGL::addWall(size_t x, size_t z, size_t wallTexture, size_t width,
                    size_t depth, size_t maxHeight,
                    std::vector<std::vector<uint32_t>> &walls) {
  gameObjects.emplace_back(
      GameObject{static_cast<float>(x), static_cast<float>(z),
                 static_cast<float>(x) + 1.0f, static_cast<float>(z) + 1.0f});
  if (z != depth - 1 && walls[z + 1][x] == 0) {
    for (size_t height = 0; height < maxHeight; height++) {
      addQuad(glm::vec3(0.0f + x, static_cast<float>(height), 0.0f + z), 0.0f,
              glm::vec3(1.0f), 1.0f, wallTexture);
    }
  }
  if (x != width - 1 && walls[z][x + 1] == 0) {
    for (size_t height = 0; height < maxHeight; height++) {
      addQuad(glm::vec3(0.5f + x, static_cast<float>(height), -0.5f + z), 90.0f,
              glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, wallTexture);
    }
  }
  if (x != 0 && walls[z][x - 1] == 0) {
    for (size_t height = 0; height < maxHeight; height++) {
      addQuad(glm::vec3(-0.5f + x, static_cast<float>(height), -0.5f + z),
              270.0f, glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, wallTexture);
    }
  }
  if (z != 0 && walls[z - 1][x] == 0) {
    for (size_t height = 0; height < maxHeight; height++) {
      addQuad(glm::vec3(0.0f + x, static_cast<float>(height), -1.0f + z),
              180.0f, glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, wallTexture);
    }
  }
}

void OpeGL::addQuad(glm::vec3 position, float angle, glm::vec3 rotation,
                    float scale, size_t texture) {
  quad.addQuad(position, angle, rotation, scale,
               static_cast<uint32_t>(texture));
}

size_t OpeGL::addTexture(const std::string &filename) {
  return textures.loadTexture(filename);
}

void APIENTRY OpeGL::glDebugOutput(GLenum source, GLenum type, unsigned int id,
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

void OpeGL::setPlayerPos(glm::vec2 pos) { camera.setPlayerPos(pos); }

void OpeGL::addPointLight(PointLight &light) { pointLights.push_back(light); }

void OpeGL::loadLevel(std::string path, uint32_t wallTexture,
                      uint32_t floorTexture, uint32_t ceilingTexture,
                      size_t maxHeight) {
  auto newLevel = std::make_unique<OpeLevel>(
      OpeLevel(path, wallTexture, floorTexture, ceilingTexture, maxHeight));
  currentLevel = std::move(newLevel);
  currentLevel->loadLevel(*this);
}

void OpeGL::addModel(std::string modelPath, glm::vec3 pos, float angle,
                     glm::vec3 rotation, float scale) {
  OpeModel model(modelPath, pos, angle, rotation, scale);
  model.loadModel();
  auto diffusePath = model.getDiffusePath();
  auto diffuse = addTexture(diffusePath);
  model.setDiffuse(diffuse);
  models.push_back(model);
}

bool OpeGL::checkCollision() {
  for (auto &gameObject : gameObjects)
    return gameObject.intersects(getPlayer());

  return false;
}

GameObject OpeGL::getPlayer() {
  auto pos = camera.getPosition();
  return GameObject{pos.x, pos.z, pos.x + 1.0f, pos.z + 1.0f};
}

void OpeGL::addCube(size_t x, size_t z, size_t texture) {
  addQuad(glm::vec3(0.0f + x, 0.0f, 0.0f + z), 0.0f, glm::vec3(1.0f), 1.0f,
          texture);
  addQuad(glm::vec3(0.5f + x, 0.0f, -0.5f + z), 90.0f,
          glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, texture);
  addQuad(glm::vec3(-0.5f + x, 0.0f, -0.5f + z), 270.0f,
          glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, texture);
  addQuad(glm::vec3(0.0f + x, 0.0f, -1.0f + z), 180.0f,
          glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, texture);
  addQuad(glm::vec3(x, 0.0f, -1.0f + z), 90.0f, glm::vec3(1.0f, 0.0f, 0.0f),
          1.0f, texture);
  addQuad(glm::vec3(x, 1.0f, z), 270.0f, glm::vec3(1.0f, 0.0f, 0.0f), 1.0f,
          texture);
}
