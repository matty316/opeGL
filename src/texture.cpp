#include "texture.hpp"

#include <format>
#include <stb_image.h>

GLuint OpeTexture::loadTexture() {
  if (loadedTextures.contains(filename))
    return loadedTextures[filename];

  unsigned int textureID;
  glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

  int width, height, nrComponents;
  unsigned char *data =
      stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
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
    stbi_image_free(data);
    auto errorMessage =
        std::format("failed to load texture at path: {}", filename);
    throw std::runtime_error(errorMessage);
  }

  loadedTextures[filename] = textureID;
  return textureID;
}
