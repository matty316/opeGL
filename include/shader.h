#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

GLuint createShader(const char *vertextPath, const char *fragmentPath);

void use(GLuint shader);
void setBool(GLuint shader, const std::string &name, bool value);
void setInt(GLuint shader, const std::string &name, int value);
void setFloat(GLuint shader, const std::string &name, float value);
void setMat4(GLuint shader, const std::string &name, glm::mat4);
void setVec3(GLuint shader, const std::string &name, glm::vec3);
void setPointLight(GLuint shader, glm::vec3 pos, int index);
void setDirLight(GLuint shader, glm::vec3 dir);
