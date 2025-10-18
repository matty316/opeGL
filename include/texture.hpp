#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <vector>

class OpeTexture {
public:
  size_t loadTexture(const std::string &filename);

private:
  std::string filename;
  static std::unordered_map<std::string, size_t> loadedTextures;
  static std::vector<GLuint> textures;
};
