#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>

class OpeTexture {
public:
  OpeTexture(const std::string &filename) : filename(filename) {}
  GLuint loadTexture();

private:
  std::string filename;
  static std::unordered_map<std::string, GLuint> loadedTextures;
};
