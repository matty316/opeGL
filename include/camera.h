#pragma once

#include <glm/glm.hpp>

enum CameraMovement {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
};

glm::vec3 cameraPos();
float getZoom();
void createCamera(glm::vec3 position);
glm::mat4 getView();
void processKeyboard(CameraMovement dir, float deltaTime);
void processMouseMovement(float xoffset, float yoffset,
                          bool constainPitch = true);
void processMouseScroll(float yoffset);

