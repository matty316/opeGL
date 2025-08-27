#include "scene.h"
#include <assimp/scene.h>

glm::mat4 toMat4(const aiMatrix4x4 &m) {
  glm::mat4 mm;
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      mm[i][j] = m[i][j];
  return mm;
}

int addNode(Scene &scene, int parent, int level) {
  int node = (int)scene.hierarchy.size();
  scene.localTransforms.push_back(glm::mat4(1.f));
  scene.globalTransforms.push_back(glm::mat4(1.f));
  scene.hierarchy.push_back({.parent = parent});
  if (parent > -1) {
    int s = scene.hierarchy[parent].firstChild;
    if (s == -1) {
      scene.hierarchy[parent].firstChild = node;
      scene.hierarchy[node].lastSibling = node;
    } else {
      int dest = scene.hierarchy[s].lastSibling;
      if (dest <= -1) {
        for (dest = s; scene.hierarchy[dest].nextSibling != -1; dest = scene.hierarchy[dest].nextSibling);
      }
      scene.hierarchy[dest].nextSibling = node;
      scene.hierarchy[s].lastSibling = node;
    }
  }
  scene.hierarchy[node].level = level;
  scene.hierarchy[node].nextSibling = -1;
  scene.hierarchy[node].firstChild = -1;
  return node;
}

void traverse(const aiScene* sourceScene, Scene& scene, aiNode* node, int parent, int atLevel) {
  int newNodeId = addNode(scene, parent, atLevel);
  if (node->mName.C_Str()) {
    uint32_t stringId = (uint32_t)scene.names.size();
    scene.names.push_back(std::string(node->mName.C_Str()));
    scene.nameForNode[newNodeId] = stringId;
  }
  for (size_t i = 0; i < node->mNumMeshes; i++) {
    int newSubNodeId = addNode(scene, newNodeId, atLevel + 1);
    uint32_t stringId = (uint32_t)scene.names.size();
    scene.names.push_back(std::string(node->mName.C_Str()) + "_Mesh_" + std::to_string(i));
    scene.nameForNode[newSubNodeId] = stringId;
    int mesh = (int)node->mMeshes[i];
    scene.meshes[newSubNodeId] = mesh;
    scene.materialForNode[newSubNodeId] = sourceScene->mMeshes[mesh]->mMaterialIndex;
    scene.globalTransforms[newSubNodeId] = glm::mat4(1.f);
    scene.localTransforms[newSubNodeId] = glm::mat4(1.f);
  }
  scene.globalTransforms[newNodeId] = glm::mat4(1.f);
  scene.localTransforms[newNodeId] = toMat4(node->mTransformation);
  for (unsigned int n = 0; n < node->mNumChildren; n++)
    traverse(sourceScene, scene, node->mChildren[n], newNodeId, atLevel + 1);
}
