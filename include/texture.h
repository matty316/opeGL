#pragma once
#include <glad/glad.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

struct Texture {
  unsigned int ID;
  int TEXTURE_NUM;

  Texture(const char *path, int num, bool isPNG = false) {
    TEXTURE_NUM = num;
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                   isPNG ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
      glGenerateTextureMipmap(GL_TEXTURE_2D);
    } else {
      std::cerr << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
  }

  void bind() {
    glActiveTexture(GL_TEXTURE0 + TEXTURE_NUM);
    glBindTexture(GL_TEXTURE_2D, ID);
  }
};
