#include "camera.h"
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(glm::vec3 position) {
  pos = position;
  worldUp = up;
  updateCameraVecs();
}

glm::mat4 Camera::getView() { return glm::lookAt(pos, pos + front, up); }

void Camera::processKeyboard(CameraMovement dir, float deltaTime) {
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

void Camera::processMouseMovement(float xoffset, float yoffset,
                                  bool constainPitch) {
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

void Camera::processMouseScroll(float yoffset) {
  zoom -= (float)yoffset;
  if (zoom < 1.0f)
    zoom = 1.0f;
  if (zoom > 45.0f)
    zoom = 45.0f;
}

void Camera::updateCameraVecs() {
  glm::vec3 f;
  f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  f.y = sin(glm::radians(pitch));
  f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  front = glm::normalize(f);
  right = glm::normalize(glm::cross(front, worldUp));
  up = glm::normalize(glm::cross(right, front));
}
