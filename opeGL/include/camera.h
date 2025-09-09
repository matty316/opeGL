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

enum CameraType {
  Fly, FPS
};

void createCamera(CameraType type, glm::vec3 pos);
void updateCamera(CameraMovement movement, float deltaTime, const glm::vec2& mousePos, bool mousePressed);
glm::mat4 getViewMatrix();
glm::vec3 getCameraPos();
