#pragma once

#include <texture.h>

struct Material {
  Material(Texture diffuse, Texture specular, float shininess)
      : diffuse(diffuse), specular(specular), shininess(shininess) {}

private:
  Texture diffuse;
  Texture specular;
  float shininess;
};
