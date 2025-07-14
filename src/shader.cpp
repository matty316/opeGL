#include "shader.h"
#include "glm/fwd.hpp"
#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
  std::ifstream vertexFile(vertexPath);
  std::ifstream fragmentFile(fragmentPath);

  if (vertexFile.is_open() && fragmentFile.is_open()) {
    std::stringstream vertexBuffer;
    std::stringstream fragmentBuffer;

    vertexBuffer << vertexFile.rdbuf();
    fragmentBuffer << fragmentFile.rdbuf();

    std::string vertexCode = vertexBuffer.str();
    std::string fragmentCode = fragmentBuffer.str();

    vertexFile.close();
    fragmentFile.close();

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
      std::cerr << "Error::Shader::Vertex::CompilationFailed\n"
                << vertexPath << infoLog << std::endl;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
      std::cerr << "Error::Shader::Fragment::CompilationFailed\n"
                << fragmentPath << infoLog << std::endl;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(ID, 512, nullptr, infoLog);
      std::cerr << "Error::Shader::Program::LinkingFailed\n"
                << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
  } else {
    std::cerr << "Error::Shader::FileNotSuccessfullyRead" << std::endl;
  }
}

void Shader::use() { glUseProgram(ID); }

void Shader::setBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string &name, glm::mat4 matrix) const {
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                     glm::value_ptr(matrix));
}

void Shader::setVec3(const std::string &name, glm::vec3 vec) const {
  glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec));
}

void Shader::setPointLight(glm::vec3 pos, int index) {
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

  setVec3(position.str(), pos);
  setVec3(ambient.str(), glm::vec3{0.5f});
  setVec3(diffuse.str(), glm::vec3{0.8f});
  setVec3(specular.str(), glm::vec3{1.0f});
  setFloat(constant.str(), 1.0f);
  setFloat(linear.str(), 0.09f);
  setFloat(quadratic.str(), 0.032f);
}

void Shader::setDirLight(glm::vec3 dir) {
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

  setVec3(direction.str(), dir);
  setVec3(ambient.str(), glm::vec3{0.05});
  setVec3(diffuse.str(), glm::vec3{0.4f});
  setVec3(specular.str(), glm::vec3{0.5f});
}
