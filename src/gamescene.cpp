#include "gamescene.h"
#include "camera.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "model.h"
#include "plane.h"
#include "shader.h"
#include "stb_image.h"

#include <X11/Xlib.h>
#include <cstddef>
#include <iostream>
#include <vector>

int screenWidth = 800;
int screenHeight = 600;

float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

unsigned int depthMapFBO, depthMap;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

std::vector<Model> models;

Camera cam{glm::vec3{0.0f, 0.0f, 3.0f}};

std::vector<glm::vec3> pLightPositions{glm::vec3{0.7f, 0.2f, 2.0f}};
glm::vec3 dirLight{-0.2f, -1.0f, -0.3f};

unsigned int skyboxVAO, skyboxVBO, skyboxTexture;

GLuint shader, skyboxShader, depthShader;

void setupSkyboxVAO();
unsigned int loadSkybox();
void setupDepthMap();
void renderDepthMap();

void createScene() {
  shader = createShader("resources/shader.vert", "resources/shader.frag");
  skyboxShader = createShader("resources/skybox.vert", "resources/skybox.frag");
  depthShader = createShader("resources/shadow.vert", "resources/shadow.frag");
  use(shader);
  setInt(shader, "numOfPointLights", pLightPositions.size());
  setDirLight(shader, dirLight);
  for (size_t i = 0; i < pLightPositions.size(); i++) {
    setPointLight(shader, pLightPositions[i], i);
  }
  setInt(depthShader, "shadowMap", 0);

  Model backpack{"resources/backpack.obj", glm::vec3{0.0f, -1.0f, 0.0f},
                 glm::vec3{1.0f}, 0.0f, 0.5f};
  models.push_back(backpack);

  skyboxTexture = loadSkybox();
  setupSkyboxVAO();
  use(skyboxShader);
  setInt(skyboxShader, "skybox", 0);

  createPlane("resources/textures/rocky_terrain_02_diff_4k.png", "resources/textures/rocky_terrain_02_diff_4k.png");
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

void renderModels(GLuint shader) {
  for (size_t i = 0; i < models.size(); i++) {
    models[i].draw(shader);
  }

  drawPlane(shader);
}

void renderScene(GLFWwindow *window) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  renderDepthMap();

  use(shader);
  setVec3(shader, "viewPos", cam.pos);
  setFloat(shader, "material.shininess", 32.0f);

  auto aspect = (float)screenWidth / (float)screenHeight;
  auto projection =
      glm::perspective(glm::radians(cam.zoom), aspect, 0.1f, 100.0f);
  setMat4(shader, "projection", projection);

  auto view = cam.getView();
  setMat4(shader, "view", view);

  renderModels(shader);

  // draw skybox as last
  glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when
                          // values are equal to depth buffer's content
  use(skyboxShader);
  view = glm::mat4(
      glm::mat3(cam.getView())); // remove translation from the view matrix
  setMat4(skyboxShader, "view", view);
  setMat4(skyboxShader, "projection", projection);
  // skybox cube
  glBindVertexArray(skyboxVAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glDepthFunc(GL_LESS); // set depth function back to default

  glfwSwapBuffers(window);
  glfwPollEvents();
}

unsigned int loadSkybox() {
  std::vector<std::string> faces{
      "resources/skybox/right.jpg", "resources/skybox/left.jpg",
      "resources/skybox/top.jpg",   "resources/skybox/bottom.jpg",
      "resources/skybox/front.jpg", "resources/skybox/back.jpg"};

  stbi_set_flip_vertically_on_load_thread(false);
  unsigned int textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  int width, height, nrChannels;
  for (unsigned int i = 0; i < faces.size(); i++) {
    unsigned char *data =
        stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width,
                   height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      stbi_image_free(data);
    } else {
      std::cout << "Cubemap tex failed to load at path: " << faces[i]
                << std::endl;
      stbi_image_free(data);
    }
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  stbi_set_flip_vertically_on_load_thread(true);
  return textureID;
}

void setupSkyboxVAO() {
  const float skyboxVertices[] = {
      // positions
      -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

      -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
      -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

      1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

      -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

      -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

      -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

  glGenVertexArrays(1, &skyboxVAO);
  glGenBuffers(1, &skyboxVBO);
  glBindVertexArray(skyboxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
}

void setupDepthMap() {
  glGenFramebuffers(1, &depthMapFBO);

  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,
               SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderDepthMap() {
  auto lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
  auto lightView =
      glm::lookAt(dirLight, glm::vec3{0.0f}, glm::vec3{0.0f, 1.0f, 0.0f});
  auto lightSpaceMatrix = lightProjection * lightView;

  use(depthShader);
  setMat4(depthShader, "lightSpaceMatrix", lightSpaceMatrix);

  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glClear(GL_DEPTH_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE0);

  renderModels(depthShader);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, screenWidth, screenHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  use(shader);
  setMat4(shader, "lightSpaceMatrix", lightSpaceMatrix);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, depthMap);
}
