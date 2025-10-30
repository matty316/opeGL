#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

layout (location = 0) out vec2 texCoord;
layout (location = 1) out uint textureIndex;
layout (location = 2) out vec3 normal;
layout (location = 3) out vec3 fragPos;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 modelMatrix;
uniform uint textureIdx;

void main() {
  fragPos = vec3(modelMatrix * vec4(aPos, 1.0));
  texCoord = aTexCoord;
  textureIndex = textureIdx;
  normal = mat3(transpose(inverse(modelMatrix))) * aNormal;
  gl_Position = projection * view * vec4(fragPos, 1.0);
}
