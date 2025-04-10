#pragma once

#include "glad/glad.h"
#include "glm/fwd.hpp"
#include "lighting.h"
#include <string>

struct Shader {
public:
  unsigned int ID;

  Shader(const char *vertexPath, const char *fragmentPath);
  void use();
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  void setMat4(const std::string &name, glm::mat4) const;
  void setVec3(const std::string &name, glm::vec3) const;
  void setPointLight(PointLight light, int index);
  void setDirLight(DirectionalLight light);
};
