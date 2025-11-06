#pragma once

#include "game-object.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

enum OpeCameraType { FLY, FPS };

class OpeCamera {
public:
  OpeCamera() = default;
  OpeCamera(const glm::vec3 &pos, const glm::vec3 &target, const glm::vec3 &up,
            OpeCameraType cameraType = FPS)
      : cameraPos(pos), cameraOrientation(glm::lookAt(pos, target, up)),
        worldUp(up), cameraType(cameraType) {}
  void update(double deltaTime, const glm::vec2 &mousePos, bool collided);
  glm::mat4 getView();
  glm::vec3 getPosition();
  void setPlayerPos(glm::vec2 pos);
  void resolveCollision(const OpeGameObject &gameObject);
  OpeGameObject getPlayer();
  void updateRightAxes(double deltaTime, float x, float y);

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
  OpeCameraType cameraType = FPS;
  void setPosition(const glm::vec3 &pos);
  void setUpVector(glm::vec3 up);
  void resetMousePosition(const glm::vec2 &p);
};
