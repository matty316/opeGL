#include "texture.h"
#include <print>
#include <stb_image.h>
#include <string>
#include <unordered_map>

std::unordered_map<std::string, GLuint> loadedTextures;
std::unordered_map<std::string, size_t> loadedBindlessTextures;
std::vector<GLuint64> handles;

GLuint loadTexture(const char *path) {
  if (loadedTextures.contains(std::string(path)))
    return loadedTextures[std::string(path)];

  unsigned int textureID;
  glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

  int width, height, nrComponents;
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format = GL_NONE;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureStorage2D(textureID, 1, GL_RGBA8, width, height);
    glTextureSubImage2D(textureID, 0, 0, 0, width, height, format,
                        GL_UNSIGNED_BYTE, data);

    glGenerateTextureMipmap(textureID);

    stbi_image_free(data);
  } else {
    std::println("Texture failed to load at path: {}", path);
    stbi_image_free(data);
  }

  loadedTextures[std::string(path)] = textureID;
  return textureID;
}

