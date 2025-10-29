#include "shader.hpp"
#include "frag-shader.h"
#include "light.hpp"
#include "texture.hpp"
#include "vert-shader.h"
#include <format>
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

void OpeShader::setTextures(int numTextures) {
  textureLocation = glGetUniformLocation(ID, "textures");
  std::vector<int> textureUnits;
  textureUnits.resize(numTextures);
  for (int i = 0; i < numTextures; i++) {
    textureUnits[i] = i;
  }
  glUniform1iv(textureLocation, numTextures, textureUnits.data());
}
void OpeShader::cleanup() { glDeleteProgram(ID); }

void OpeShader::setPointLight(PointLight &light, size_t index) {
  std::string lightName = std::format("pointLights[{}]", index);

  std::string lightPositionName = std::format("{}.position", lightName);
  setVec3(lightPositionName, light.position);

  std::string lightAmbientName = std::format("{}.ambient", lightName);
  setVec3(lightAmbientName, light.ambient);

  std::string lightDiffuseName = std::format("{}.diffuse", lightName);
  setVec3(lightDiffuseName, light.diffuse);

  std::string lightSpecularName = std::format("{}.specular", lightName);
  setVec3(lightSpecularName, light.specular);

  std::string lightConstantName = std::format("{}.constant", lightName);
  setFloat(lightConstantName, light.constant);

  std::string lightLinearName = std::format("{}.linear", lightName);
  setFloat(lightLinearName, light.linear);

  std::string lightQuadraticName = std::format("{}.quadratic", lightName);
  setFloat(lightQuadraticName, light.quadratic);
}
