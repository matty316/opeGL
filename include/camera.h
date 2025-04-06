#pragma once

#include "glm/ext/matrix_transform.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovement {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
};

struct Camera {
public:
  float zoom = 45.0f;
  Camera(glm::vec3 position) {
    pos = position;
    worldUp = up;
    updateCameraVecs();
  }

  glm::mat4 getView() { return glm::lookAt(pos, pos + front, up); }

  void processKeyboard(CameraMovement dir, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    if (dir == FORWARD)
      pos += front * velocity;
    if (dir == BACKWARD)
      pos -= front * velocity;
    if (dir == LEFT)
      pos -= right * velocity;
    if (dir == RIGHT)
      pos += right * velocity;
  }

  void processMouseMovement(float xoffset, float yoffset,
                            GLboolean constainPitch = true) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (constainPitch) {
      if (pitch > 89.0f)
        pitch = 89.0f;
      if (pitch < -89.0f)
        pitch = -89.0f;
    }

    updateCameraVecs();
  }

  void processMouseScroll(float yoffset) {
    zoom -= (float)yoffset;
    if (zoom < 1.0f)
      zoom = 1.0f;
    if (zoom > 45.0f)
      zoom = 45.0f;
  }

private:
  glm::vec3 pos{0.0f, 0.0f, 0.0f};
  glm::vec3 front{0.0f, 0.0f, -1.0f};
  glm::vec3 up{0.0f, 1.0f, 0.0f};
  glm::vec3 right;
  glm::vec3 worldUp;

  float yaw = -90.0f;
  float pitch = 0.0f;
  float movementSpeed = 2.5f;
  float mouseSensitivity = 0.1f;

  void updateCameraVecs() {
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(f);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
  }
};
