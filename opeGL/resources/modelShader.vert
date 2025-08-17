#version 460 core
layout(std140, binding = 0) uniform PerFrameData
{
  uniform mat4 mvp;
  uniform int isWireframe;
};
layout(location=0) in vec3 pos;
layout(location=0) out vec3 color;

void main() {
  gl_Position = mvp * vec4(pos, 1.0);
  color = isWireframe > 0 ? vec3(0.0f) : pos.xyz;
}
