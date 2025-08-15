#pragma once

#include <glm/glm.hpp>

enum CameraType {
  FLY,
  FPS,
};

enum CameraMovement {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
};

glm::vec3 cameraPos();
float getZoom();
void createCamera(glm::vec3 position, CameraType type = FPS);
glm::mat4 getView();
void processKeyboard(CameraMovement dir, float deltaTime);
void processMouseMovement(float xoffset, float yoffset,
                          bool constainPitch = true);
void processMouseScroll(float yoffset);

