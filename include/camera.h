#pragma once

#include <glm/glm.hpp>

enum CameraMovement {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
};

class Camera {
public:
  glm::vec3 pos{0.0f, 0.0f, 0.0f};
  float zoom = 45.0f;
  Camera(glm::vec3 position);
  glm::mat4 getView();
  void processKeyboard(CameraMovement dir, float deltaTime);
  void processMouseMovement(float xoffset, float yoffset,
                            bool constainPitch = true);
  void processMouseScroll(float yoffset);

private:
  glm::vec3 front{0.0f, 0.0f, -1.0f};
  glm::vec3 up{0.0f, 1.0f, 0.0f};
  glm::vec3 right;
  glm::vec3 worldUp;

  float yaw = -90.0f;
  float pitch = 0.0f;
  float movementSpeed = 2.5f;
  float mouseSensitivity = 0.1f;

  void updateCameraVecs();
};
