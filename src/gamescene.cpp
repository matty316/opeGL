#include "gamescene.h"
#include "camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include "model.h"
#include "plane.h"
#include "shader.h"
#include "stb_image.h"

#include <iostream>
#include <vector>

int screenWidth = 800;
int screenHeight = 600;

float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

std::vector<Model> models;

Camera cam{glm::vec3{0.0f, 0.0f, 3.0f}};

std::vector<glm::vec3> pLightPositions{glm::vec3{0.7f, 0.2f, 2.0f}};

unsigned int skyboxVAO, skyboxVBO, skyboxTexture;

void setupSkyboxVAO();
unsigned int loadSkybox();

void createScene(Shader &shader, Shader &skyboxShader) {
  shader.use();
  shader.setInt("numOfPointLights", pLightPositions.size());
  shader.setDirLight(glm::vec3{-0.2f, -1.0f, -0.3f});
  for (size_t i = 0; i < pLightPositions.size(); i++) {
    shader.setPointLight(pLightPositions[i], i);
  }

  Model backpack{"resources/backpack.obj", glm::vec3{0.0f, -1.0f, 0.0f},
                 glm::vec3{1.0f}, 0.0f, 0.5f};
  models.push_back(backpack);

  skyboxTexture = loadSkybox();
  setupSkyboxVAO();
  skyboxShader.use();
  skyboxShader.setInt("skybox", 0);
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

void renderScene(GLFWwindow *window, Shader &shader, Shader &skyboxShader,
                 Plane &ground) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shader.use();
  shader.setVec3("viewPos", cam.pos);
  shader.setFloat("material.shininess", 32.0f);

  auto aspect = (float)screenWidth / (float)screenHeight;
  auto projection =
      glm::perspective(glm::radians(cam.zoom), aspect, 0.1f, 100.0f);
  shader.setMat4("projection", projection);

  auto view = cam.getView();
  shader.setMat4("view", view);
  for (size_t i = 0; i < models.size(); i++) {
    models[i].draw(shader);
  }

  ground.draw(shader);

  // draw skybox as last
  glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when
                          // values are equal to depth buffer's content
  skyboxShader.use();
  view = glm::mat4(
      glm::mat3(cam.getView())); // remove translation from the view matrix
  skyboxShader.setMat4("view", view);
  skyboxShader.setMat4("projection", projection);
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
  float skyboxVertices[] = {
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
