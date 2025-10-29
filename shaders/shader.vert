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

struct PerInstanceData {
  mat4 modelMatrix;
  uint textureIndex;
};

layout(binding = 2, std430) readonly buffer ssbo1 {
  PerInstanceData data[];
};

void main() {
  mat4 model = data[gl_InstanceID].modelMatrix;
  fragPos = vec3(model * vec4(aPos, 1.0));
  texCoord = aTexCoord;
  textureIndex = data[gl_InstanceID].textureIndex;
  normal = mat3(transpose(inverse(model))) * aNormal;
  gl_Position = projection * view * vec4(fragPos, 1.0);
}
