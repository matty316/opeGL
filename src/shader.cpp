#include "shader.h"
#include "glm/fwd.hpp"
#include <cstdlib>
#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <print>
#include <sstream>
#include <string>

GLuint compileShader(const char *path, GLenum type) {
  std::ifstream file(path);
  if (file.is_open()) {
    std::stringstream buffer;

    buffer << file.rdbuf();

    auto codeString = buffer.str();

    file.close();

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
      std::println("Error compiling shader at {}, info: {}", path, infoLog);
      exit(EXIT_FAILURE);
    }
    return shader;
  }
}

GLuint createShader(const char *vertexPath, const char *fragmentPath, const char *geometryPath) {
  auto vertex = compileShader(vertexPath, GL_VERTEX_SHADER);
  auto fragment = compileShader(fragmentPath, GL_FRAGMENT_SHADER);
  GLuint geometry;

  if (geometryPath)
    geometry = compileShader(geometryPath, GL_GEOMETRY_SHADER);

  auto program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  if (geometryPath)
    glAttachShader(program, geometry);
  glLinkProgram(program);

  int success;
  char infoLog[512];

  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, nullptr, infoLog);
    std::println("Error linking program {}", infoLog);
    exit(EXIT_FAILURE);
  }
  return program;
}

void use(GLuint program) { glUseProgram(program); }

void setBool(GLuint program, const std::string &name, bool value) {
  glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
}

void setInt(GLuint program, const std::string &name, int value) {
  glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}

void setFloat(GLuint program, const std::string &name, float value) {
  glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}

void setMat4(GLuint program, const std::string &name, glm::mat4 matrix) {
  glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE,
                     glm::value_ptr(matrix));
}

void setVec3(GLuint program, const std::string &name, glm::vec3 vec) {
  glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, glm::value_ptr(vec));
}

void setPointLight(GLuint program, glm::vec3 pos, int index) {
  std::stringstream name;
  name << "pointLights[" << index << "].";

  std::stringstream position;
  position << name.str() << "position";

  std::stringstream ambient;
  ambient << name.str() << "ambient";

  std::stringstream diffuse;
  diffuse << name.str() << "diffuse";

  std::stringstream specular;
  specular << name.str() << "specular";

  std::stringstream constant;
  constant << name.str() << "constant";

  std::stringstream linear;
  linear << name.str() << "linear";

  std::stringstream quadratic;
  quadratic << name.str() << "quadratic";

  setVec3(program, position.str(), pos);
  setVec3(program, ambient.str(), glm::vec3{0.5f});
  setVec3(program, diffuse.str(), glm::vec3{0.8f});
  setVec3(program, specular.str(), glm::vec3{1.0f});
  setFloat(program, constant.str(), 1.0f);
  setFloat(program, linear.str(), 0.09f);
  setFloat(program, quadratic.str(), 0.032f);
}

void setDirLight(GLuint program, glm::vec3 dir) {
  std::stringstream name;
  name << "dirLight.";

  std::stringstream direction;
  direction << name.str() << "direction";

  std::stringstream ambient;
  ambient << name.str() << "ambient";

  std::stringstream diffuse;
  diffuse << name.str() << "diffuse";

  std::stringstream specular;
  specular << name.str() << "specular";

  setVec3(program, direction.str(), dir);
  setVec3(program, ambient.str(), glm::vec3{0.05});
  setVec3(program, diffuse.str(), glm::vec3{0.4f});
  setVec3(program, specular.str(), glm::vec3{0.5f});
}
