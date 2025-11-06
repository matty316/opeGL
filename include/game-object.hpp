#pragma once

#include <glm/glm.hpp>

enum OpeGameObjectType { PLAYER, WALL };

struct OpeGameObject {
  glm::vec3 pos;
  glm::vec3 size;
  OpeGameObjectType type;

  OpeGameObject(glm::vec3 pos, glm::vec3 size, OpeGameObjectType type)
      : pos(pos), size(size), type(type) {}

  bool intersects(const OpeGameObject &other) const {
    return (pos.x < other.pos.x + other.size.x &&
            pos.x + size.x > other.pos.x &&
            pos.z < other.pos.z + other.size.z && pos.z + size.z > other.pos.z);
  }
};
