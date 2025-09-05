#include "gamescene.h"
#include "camera.h"
#include "chunk.h"
#include "cube.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "model.h"
#include "plane.h"
#include "shader.h"
#include "stb_image.h"

#include <X11/Xlib.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <print>
#include <vector>

int screenWidth = 800;
int screenHeight = 600;

float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

unsigned int depthMapFBO, depthMap;

float lastFrame = 0.0f;

std::vector<Model> models;
std::vector<Plane> planes;
std::vector<Cube> cubes;
std::vector<Chunk> chunks;

std::vector<glm::vec3> pLightPositions{glm::vec3{0.7f, 0.2f, 2.0f}};
glm::vec3 dirLight{-2.0f, 4.0f, -1.0f};

GLuint skyboxVAO, skyboxVBO, skyboxTexture, debugShadowShader, modelShader;
GLuint shader, skyboxShader, depthShader;
glm::mat4 lightSpaceMatrix;

void setupSkyboxVAO();
unsigned int loadSkybox();
void setupDepthMap();
void renderDepthMap(float nearPlane, float farPlane);
void renderDebugQuad(float nearPlane, float farPlane);

void createScene() {
  shader = createShader("resources/shader.vert", "resources/shader.frag");
  skyboxShader = createShader("resources/skybox.vert", "resources/skybox.frag");
  depthShader = createShader("resources/shadow.vert", "resources/shadow.frag");
  debugShadowShader =
      createShader("resources/shadowDebug.vert", "resources/shadowDebug.frag");
  modelShader =
      createShader("resources/modelShader.vert", "resources/modelShader.frag");

  setupDepthMap();
  use(shader);
  setDirLight(shader, dirLight);
  setInt(shader, "numOfPointLights", pLightPositions.size());
  setInt(shader, "shadowMap", 11);
  for (size_t i = 0; i < pLightPositions.size(); i++) {
    setPointLight(shader, pLightPositions[i], i);
  }

  use(debugShadowShader);
  setInt(debugShadowShader, "depthMap", 0);

  skyboxTexture = loadSkybox();
  setupSkyboxVAO();
  use(skyboxShader);
  setInt(skyboxShader, "skybox", 0);
}

void addCube(GLuint diff, GLuint spec, glm::vec3 pos, glm::vec3 rotation,
             float angle, float scale) {
  Cube cube = createCube(diff, spec, pos, rotation, angle, scale);
  cubes.push_back(cube);
}

void updateScene(int width, int height) {
  screenWidth = width;
  screenHeight = height;
}

void addModel(const char *path, glm::vec3 pos, glm::vec3 rotation, float angle,
              float scale) {
  Model model = createModel(path, pos, rotation, angle, scale);
  models.push_back(model);
}

void addPlane(const char *diffusePath, const char *specularPath, glm::vec3 pos,
              glm::vec3 rotation, float angle, float scale, int tiling) {
  Plane plane = createPlane(diffusePath, specularPath, pos, rotation, angle,
                            scale, tiling);
  planes.push_back(plane);
}

void renderModels(GLuint shader) {
  for (auto &plane : planes)
    drawPlane(plane, shader);
  for (auto &model : models)
    drawModel(model, shader);
  for (auto &cube : cubes)
    drawCube(cube, shader);
  for (auto &chunk : chunks)
    drawChunk(chunk, shader);
}

void renderScene(GLFWwindow *window) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  float nearPlane = 1.0f, farPlane = 7.5f;
  renderDepthMap(nearPlane, farPlane);

  use(shader);
  setVec3(shader, "viewPos", getCameraPos());
  setFloat(shader, "material.shininess", 32.0f);

  auto aspect = (float)screenWidth / (float)screenHeight;
  auto projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
  setMat4(shader, "projection", projection);

  auto view = getViewMatrix();
  setMat4(shader, "view", view);
  setMat4(shader, "lightSpaceMatrix", lightSpaceMatrix);

  use(shader);

  glActiveTexture(GL_TEXTURE11);
  glBindTexture(GL_TEXTURE_2D, depthMap);

  renderModels(shader);

  // draw skybox as last
  glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when
                          // values are equal to depth buffer's content
  use(skyboxShader);
  view = glm::mat4(
      glm::mat3(getViewMatrix())); // remove translation from the view matrix
  setMat4(skyboxShader, "view", view);
  setMat4(skyboxShader, "projection", projection);
  // skybox cube
  glBindVertexArray(skyboxVAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glDepthFunc(GL_LESS); // set depth function back to default

  // renderDebugQuad(nearPlane, farPlane);
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
               SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderDepthMap(float nearPlane, float farPlane) {
  auto lightProjection =
      glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
  auto lightView =
      glm::lookAt(dirLight, glm::vec3{0.0f}, glm::vec3{0.0f, 1.0f, 0.0f});
  lightSpaceMatrix = lightProjection * lightView;

  use(depthShader);
  setMat4(depthShader, "lightSpaceMatrix", lightSpaceMatrix);

  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glClear(GL_DEPTH_BUFFER_BIT);

  renderModels(depthShader);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, screenWidth, screenHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  use(shader);
  setMat4(shader, "lightSpaceMatrix", lightSpaceMatrix);
  glActiveTexture(GL_TEXTURE0);
}

void renderDebugQuad(float nearPlane, float farPlane) {
  use(debugShadowShader);
  setFloat(debugShadowShader, "near_plane", nearPlane);
  setFloat(debugShadowShader, "far_plane", farPlane);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, depthMap);

  GLuint quadVAO = 0, quadVBO;
  if (quadVAO == 0) {
    float quadVerts[] = {
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), &quadVerts,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
  }
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void addChunk(glm::vec3 pos, glm::vec3 rotation, float angle, float scale, ChunkType type, size_t chunkSize) {
  Chunk chunk = createChunk(pos, rotation, angle, scale, type, chunkSize);
  chunks.push_back(chunk);
}
