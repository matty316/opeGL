#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

struct Hierarchy {
  int parent;
  int firstChild;
  int nextSibling;
  int lastSibling;
  int level;
};

constexpr const int MAX_NODE_LEVEL = 16;

struct Scene {
  std::vector<glm::mat4> localTransforms;
  std::vector<glm::mat4> globalTransforms;
  std::vector<int> changedAtThisFrame[MAX_NODE_LEVEL];
  std::vector<Hierarchy> hierarchy;
  std::unordered_map<uint32_t, uint32_t> meshes;
  std::unordered_map<uint32_t, uint32_t> materialForNode;
  std::unordered_map<uint32_t, uint32_t> nameForNode;
  std::vector<std::string> names;
  std::vector<std::string> materialNames;
};
