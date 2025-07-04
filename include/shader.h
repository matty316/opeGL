#pragma once

#include <glm/glm.hpp>
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
  void setPointLight(glm::vec3 pos, int index);
  void setDirLight(glm::vec3 dir);
};
