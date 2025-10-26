#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

layout (location = 0) out vec2 texCoord;
layout (location = 1) out uint textureIndex;

uniform mat4 view;
uniform mat4 projection;

struct PerInstanceData {
  mat4 modelMatrix;
  uint textureIndex;
};

layout(binding = 2, std430) readonly buffer ssbo1 {
  PerInstanceData data[];
};

void main() {
  texCoord = aTexCoord;
  textureIndex = data[gl_InstanceID].textureIndex;
  gl_Position = projection * view * data[gl_InstanceID].modelMatrix * vec4(aPos, 1.0);
}
