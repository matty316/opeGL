#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 view;
uniform mat4 projection;

layout(binding = 2, std430) readonly buffer ssbo1 {
  mat4 modelMatrices[];
};

void main() {
  gl_Position = projection * view * modelMatrices[gl_InstanceID] * vec4(aPos, 1.0);
}
