#include "camera.h"
#include <glm/ext/matrix_transform.hpp>

glm::vec3 pos{0.0f, 0.0f, 0.0f};
glm::vec3 up{0.0f, 1.0f, 0.0f};
glm::vec3 front{0.0f, 0.0f, -1.0f};
glm::vec3 worldUp;
glm::vec3 right;
float yaw = -90.0f;
float pitch = 0.0f;
float movementSpeed = 5.0f;
float mouseSensitivity = 0.1f;
float zoom = 45.0f;
CameraType cameraType;

void updateCameraVecs();

void createCamera(glm::vec3 position, CameraType type) {
  pos = position;
  cameraType = type;
  worldUp = up;
  updateCameraVecs();
}

glm::vec3 cameraPos() {
  return pos;
}

float getZoom() {
  return zoom;
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

  if (cameraType == FPS) 
    pos.y = 0.0f;
}

void processMouseMovement(float xoffset, float yoffset,
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

void processMouseScroll(float yoffset) {
  zoom -= (float)yoffset;
  if (zoom < 1.0f)
    zoom = 1.0f;
  if (zoom > 45.0f)
    zoom = 45.0f;
}

void updateCameraVecs() {
  glm::vec3 f;
  f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  f.y = sin(glm::radians(pitch));
  f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  front = glm::normalize(f);
  right = glm::normalize(glm::cross(front, worldUp));
  up = glm::normalize(glm::cross(right, front));
}
