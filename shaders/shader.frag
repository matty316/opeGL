#version 460 core

#define MAX_TEXTURES 16

layout (location = 0) in vec2 texCoord;
layout (location = 1) in flat uint textureIndex;

out vec4 FragColor;

uniform sampler2D textures[MAX_TEXTURES]; 


void main() {
  vec4 color = texture(textures[textureIndex], texCoord);
  FragColor = color;
}
