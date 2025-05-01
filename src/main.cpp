// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
// clang-format on

#include "camera.h"
#include "error.h"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include "lighting.h"
#include "model.h"
#include "shader.h"
#include <iostream>
#include <stb_image.h>

const unsigned int screenWidth = 800;
const unsigned int screenHeight = 600;

DirectionalLight dirLight{
    glm::vec3{-0.2f, -1.0f, -0.3f},
    glm::vec3{0.05f, 0.05f, 0.05f},
    glm::vec3{0.4f, 0.4f, 0.4f},
    glm::vec3{0.5f, 0.5f, 0.5f},
};

PointLight pointLight0{
    glm::vec3{0.7f, 0.2f, 2.0f},
    glm::vec3{0.05f, 0.05f, 0.05f},
    glm::vec3{0.8f, 0.8f, 0.8f},
    glm::vec3{1.0f, 1.0f, 1.0f},
};

PointLight pointLight1{
    glm::vec3{0.7f, 0.2f, 2.0f},
    glm::vec3{0.05f, 0.05f, 0.05f},
    glm::vec3{0.8f, 0.8f, 0.8f},
    glm::vec3{1.0f, 1.0f, 1.0f},
};

PointLight pointLight2{
    glm::vec3{0.7f, 0.2f, 2.0f},
    glm::vec3{0.05f, 0.05f, 0.05f},
    glm::vec3{0.8f, 0.8f, 0.8f},
    glm::vec3{1.0f, 1.0f, 1.0f},
};

PointLight pointLight3{
    glm::vec3{0.7f, 0.2f, 2.0f},
    glm::vec3{0.05f, 0.05f, 0.05f},
    glm::vec3{0.8f, 0.8f, 0.8f},
    glm::vec3{1.0f, 1.0f, 1.0f},
};

Camera cam{glm::vec3{0.0f, 0.0f, 3.0f}};
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
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

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;

  lastX = xpos;
  lastY = ypos;
  cam.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  cam.processMouseScroll(static_cast<float>(yoffset));
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

  GLFWwindow *window = glfwCreateWindow(800, 600, "OpeGL", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to init glad" << std::endl;
    return -1;
  }

  enableReportGlErrors();

  glEnable(GL_DEPTH_TEST);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  stbi_set_flip_vertically_on_load(true);

  Shader shader("../resources/shader.vert", "../resources/shader.frag");
  shader.use();
  shader.setInt("numOfPointLights", 4);

  Cube cube{shader, "../resources/container2.png",
            "../resources/container2_specular.png"};

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    processInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    shader.setVec3("viewPos", cam.pos);
    shader.setFloat("material.shininess", 32.0f);

    shader.setDirLight(dirLight);
    shader.setPointLight(pointLight0, 0);
    shader.setPointLight(pointLight1, 1);
    shader.setPointLight(pointLight2, 2);
    shader.setPointLight(pointLight3, 3);

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    auto projection = glm::perspective(
        glm::radians(cam.zoom), (float)width / (float)height, 0.1f, 100.0f);
    shader.setMat4("projection", projection);

    auto view = cam.getView();
    shader.setMat4("view", view);

    cube.render(shader);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
