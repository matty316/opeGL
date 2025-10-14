#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class OpeShader {
public:
  OpeShader();
  void use();
  void setMat4(const std::string &name, const glm::mat4 &mat) const;
  void cleanup();

private:
  GLuint ID;

  GLuint compile(unsigned char source[], unsigned int len, GLenum type);
};
