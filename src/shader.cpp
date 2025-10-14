#include "shader.hpp"
#include "frag-shader.h"
#include "vert-shader.h"
#include <stdexcept>
#include <string>

OpeShader::OpeShader() {
  auto vertex =
      compile(shaders_shader_vert, shaders_shader_vert_len, GL_VERTEX_SHADER);
  auto fragment =
      compile(shaders_shader_frag, shaders_shader_frag_len, GL_FRAGMENT_SHADER);

  ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);

  int success;
  char infoLog[512];

  glGetProgramiv(ID, GL_LINK_STATUS, &success);

  if (!success) {
    glGetProgramInfoLog(ID, 512, nullptr, infoLog);
    throw std::runtime_error(infoLog);
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

GLuint OpeShader::compile(unsigned char source[], unsigned int len,
                          GLenum type) {
  auto codeString = std::string(reinterpret_cast<char *>(source), len);
  auto code = codeString.c_str();

  GLuint shader;
  int success;
  char infoLog[512];

  shader = glCreateShader(type);
  glShaderSource(shader, 1, &code, nullptr);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
    throw std::runtime_error(infoLog);
  }
  return shader;
}

void OpeShader::use() { glUseProgram(ID); }

void OpeShader::setMat4(const std::string &name, const glm::mat4 &mat) const {
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                     &mat[0][0]);
}

void OpeShader::cleanup() { glDeleteProgram(ID); }
