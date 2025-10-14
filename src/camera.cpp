#include "camera.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/gtc/quaternion.hpp"

void OpeCamera::update(double deltaTime, const glm::vec2 &mousePos) {
  auto delta = mousePos - mousePosition;
  auto deltaQuat =
      glm::quat(glm::vec3(mouseSpeed * delta.y, mouseSpeed * delta.x, 0.0f));
  cameraOrientation = glm::normalize(deltaQuat * cameraOrientation);
  setUpVector(worldUp);
  mousePosition = mousePos;

  const auto v = glm::mat4_cast(cameraOrientation);
  const auto forward = -glm::vec3(v[0][2], v[1][2], v[2][2]);
  const auto right = glm::vec3(v[0][0], v[1][0], v[2][0]);
  const auto up = glm::cross(right, forward);

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
  if (movement.fast)
    accel *= fastCoef;

  if (accel == glm::vec3(0.0f)) {
    moveSpeed -=
        moveSpeed *
        glm::min((1.0f / damping) * static_cast<float>(deltaTime), 1.0f);
  } else {
    moveSpeed += accel * acceleration * static_cast<float>(deltaTime);
    const float maximumSpeed = movement.fast ? maxSpeed * fastCoef : maxSpeed;
    if (glm::length(moveSpeed) > maximumSpeed)
      moveSpeed = glm::normalize(moveSpeed) * maximumSpeed;
  }
  cameraPos += moveSpeed * static_cast<float>(deltaTime);
  cameraPos.y = playerHeight;
}

glm::mat4 OpeCamera::getView() {
  const auto t = glm::translate(glm::mat4(1.0f), -cameraPos);
  const auto r = glm::mat4_cast(cameraOrientation);
  return r * t;
}

glm::vec3 OpeCamera::getPosition() { return cameraPos; }

void OpeCamera::setPlayerPos(glm::vec2 pos) {
  cameraPos = glm::vec3(pos.x, playerHeight, pos.y);
}

void OpeCamera::setPosition(const glm::vec3 &pos) { cameraPos = pos; }

void OpeCamera::setUpVector(glm::vec3 up) {
  const auto view = getView();
  const auto dir = -glm::vec3(view[0][2], view[1][2], view[2][2]);
  cameraOrientation = glm::lookAt(cameraPos, cameraPos + dir, up);
}

void OpeCamera::resetMousePosition(const glm::vec2 &p) { mousePosition = p; }
