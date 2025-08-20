#include "camera.h"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/fwd.hpp"
#include <algorithm>
#include <glm/ext.hpp>
#include <glm/ext/matrix_transform.hpp>

float mouseSpeed = 4.0f;
float acceleration = 150.0f;
float damping = 0.2f;
float maxSpeed = 10.0f;
float fastCoef = 10.0f;

glm::vec2 mousePosition = glm::vec2(0.0f);
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, -3.0f);
glm::quat cameraOrientation = glm::quat(glm::vec3(0.0f));
glm::vec3 moveSpeed = glm::vec3(0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void createCamera() {
  auto target = glm::vec3(0.0f);
  cameraOrientation = glm::quat(glm::lookAt(cameraPosition, target, cameraUp));
}

void setUpVector(const glm::vec3& up) {
  const glm::mat4 view = getViewMatrix();
  const glm::vec3 dir = -glm::vec3(view[0][2], view[1][2], view[2][2]);
  cameraOrientation = glm::lookAt(cameraPosition, cameraPosition + dir, up);
}
void updateCamera(CameraMovement movement, float deltaTime,
                  const glm::vec2 &mousePos, bool mousePressed) {
  const glm::vec2 delta = mousePos - mousePosition;
  const glm::quat deltaQuat =
      glm::quat(glm::vec3(mouseSpeed * delta.y, mouseSpeed * delta.x, 0.0f));
  cameraOrientation = glm::normalize(deltaQuat * cameraOrientation);
  setUpVector(cameraUp);
  mousePosition = mousePos;

  const glm::mat4 v = glm::mat4_cast(cameraOrientation);
  const glm::vec3 forward = -glm::vec3(v[0][2], v[1][2], v[2][2]);
  const glm::vec3 right = glm::vec3(v[0][0], v[1][0], v[2][0]);
  const glm::vec3 up = glm::cross(right, forward);

  glm::vec3 accel(0.0f);
  if (movement.forward)
    accel += forward;
  if (movement.backward)
    accel -= forward;
  if (movement.left)
    accel -= right;
  if (movement.right)
    accel += right;
  if (movement.up)
    accel += up;
  if (movement.down)
    accel -= up;
  if (movement.fastSpeed)
    accel *= fastCoef;

  if (accel == glm::vec3(0.0f)) {
    moveSpeed -=
        moveSpeed *
        std::min((1.0f / damping) * deltaTime, 1.0f);
  } else {
    moveSpeed += accel * acceleration * deltaTime;
    const float maximumSpeed =
        movement.fastSpeed ? maxSpeed * fastCoef : maxSpeed;
    if (glm::length(moveSpeed) > maxSpeed)
      moveSpeed = glm::normalize(moveSpeed) * maximumSpeed;
  }

  cameraPosition.y = 0.0f;
  cameraPosition += moveSpeed * deltaTime;
}

glm::mat4 getViewMatrix() {
  const glm::mat4 t = glm::translate(glm::mat4(1.0f), -cameraPosition);
  const glm::mat4 r = glm::mat4_cast(cameraOrientation);
  return r * t;
}

glm::vec3 getCameraPos() { return cameraPosition; }
