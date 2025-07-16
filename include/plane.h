#pragma once

#include <glad/glad.h>

void createPlane(const char *diffusePath, const char *specularPath);
void drawPlane(GLuint shader);
