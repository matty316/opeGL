#include "gamescene.h"
#include "camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/trigonometric.hpp"
#include "lighting.h"
#include "model.h"
#include "shader.h"

#include <vector>

int screenWidth = 800;
int screenHeight = 600;

float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera cam{glm::vec3{0.0f, 0.0f, 3.0f}};
DirectionalLight directionalLight{
    glm::vec3{-0.2f, -1.0f, -0.3f},
    glm::vec3{0.05f, 0.05f, 0.05f},
    glm::vec3{0.4f, 0.4f, 0.4f},
    glm::vec3{0.5f, 0.5f, 0.5f},
};
#define NUM_OF_POINT_LIGHTS 4
glm::vec3 pLightPositions[NUM_OF_POINT_LIGHTS] = {
    glm::vec3{0.7f, 0.2f, 2.0f}, glm::vec3{2.3f, -3.3f, -4.0f},
    glm::vec3{-4.0f, 2.0f, -12.0f}, glm::vec3{0.0f, 0.0f, -3.0f}};

std::vector<PointLight> pointLights;

void createScene(Shader &shader) {
  for (size_t i = 0; i < NUM_OF_POINT_LIGHTS; i++) {
    pointLights.push_back(PointLight{pLightPositions[i]});
  }
  shader.use();
  shader.setInt("numOfPointLights", NUM_OF_POINT_LIGHTS);
}

void processMouse(GLFWwindow *window, double xposIn, double yposIn) {
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = ypos - lastY;

  lastX = xpos;
  lastY = ypos;

  cam.processMouseMovement(xoffset, yoffset);
}

void processScroll(double yoffset) {
  cam.processMouseScroll(static_cast<float>(yoffset));
}

void processInput(GLFWwindow *window) {
  float currentFrame = static_cast<float>(glfwGetTime());
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cam.processKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cam.processKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cam.processKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cam.processKeyboard(RIGHT, deltaTime);
}

void updateScene(int width, int height) {
  screenWidth = width;
  screenHeight = height;
}

void renderScene(GLFWwindow *window, Shader &shader, Model &model) {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  shader.use();
  shader.setVec3("viewPos", cam.pos);
  shader.setFloat("material.shininess", 32.0f);

  shader.setDirLight(directionalLight);
  for (size_t i = 0; i < pointLights.size(); i++) {
    shader.setPointLight(pointLights[i], i);
  }

  auto aspect = (float)screenWidth / (float)screenHeight;
  auto projection =
      glm::perspective(glm::radians(cam.zoom), aspect, 0.1f, 100.0f);
  shader.setMat4("projection", projection);

  auto view = cam.getView();
  shader.setMat4("view", view);
  model.draw(shader);

  glfwSwapBuffers(window);
  glfwPollEvents();
}
