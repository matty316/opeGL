#pragma once

#include "glm/ext/matrix_transform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

class OpeCamera {
public:
  OpeCamera() = default;
  OpeCamera(const glm::vec3 &pos, const glm::vec3 &target, const glm::vec3 &up)
      : cameraPos(pos), cameraOrientation(glm::lookAt(pos, target, up)),
        worldUp(up) {}
  void update(double deltaTime, const glm::vec2 &mousePos);
  glm::mat4 getView();
  glm::vec3 getPosition();
  void setPlayerPos(glm::vec2 pos);

  struct Movement {
    bool forward = false;
    bool backward = false;
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
    bool fast = false;
  } movement;

private:
  float mouseSpeed = 4.0f;
  float acceleration = 150.0f;
  float damping = 0.2f;
  float maxSpeed = 5.0f;
  float fastCoef = 2.0f;
  float yaw = -90.0f;
  float pitch = 0.0f;
  float playerHeight = 0.75f;
  glm::vec2 mousePosition = glm::vec2(0.0f);
  glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::quat cameraOrientation = glm::quat(glm::vec3(0.0f));
  glm::vec3 moveSpeed = glm::vec3(0.0f);
  glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
  void setPosition(const glm::vec3 &pos);
  void setUpVector(glm::vec3 up);
  void resetMousePosition(const glm::vec2 &p);
};
