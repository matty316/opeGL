#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <vector>

class OpeTexture {
public:
  size_t loadTexture(const std::string &filename);
  size_t textureCount();
  GLuint textureIdAtCount(size_t i);

private:
  std::string filename;
  std::unordered_map<std::string, size_t> loadedTextures;
  std::vector<GLuint> textures;
};
