#pragma once

#include <glad/glad.h>
#include <vector>

GLuint loadTexture(const char *path);
size_t loadBindlessTexture(const char* path);
void setupTextureBuffer();
