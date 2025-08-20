#pragma once

#include <glm/glm.hpp>

struct CameraMovement {
  bool forward = false;
  bool backward = false;
  bool left = false;
  bool right = false;
  bool up = false;
  bool down = false;
  bool fastSpeed = false;
};

void createCamera(const glm::vec3 &pos, const glm::vec3 &target, const glm::vec3 &up);
void updateCamera(CameraMovement movement, double deltaTime, const glm::vec2& mousePos, bool mousePressed);
glm::mat4 getViewMatrix();
glm::vec3 getCameraPos();
