#version 460 core

float gridSize = 100;
float gridCellSize = 0.025;
layout (location = 0) out vec2 uv;
layout (location = 1) out vec2 out_camPos;
const vec3 pos[4] = vec3[4] (
  vec3(-1.0, 0.0, -1.0),
  vec3( 1.0, 0.0, -1.0),
  vec3( 1.0, 0.0,  1.0),
  vec3(-1.0, 0.0,  1.0)
);
const int indices[6] = int[6] (0, 1, 2, 2, 3, 0);

layout(std140, binding = 0) uniform GridFrameData
{
  uniform mat4 v;
  uniform mat4 p;
  uniform vec4 camPos;
};

void main() {
  vec3 vpos = pos[indices[gl_VertexID]] * gridSize;

  vpos.x += camPos.x;
  vpos.z += camPos.z;

  out_camPos = camPos.xz;

  gl_Position = p * v * vec4(vpos, 1.0);
  uv = vpos.xz;
}

