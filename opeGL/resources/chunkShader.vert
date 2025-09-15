#version 460 core
layout(location = 0) in uint data;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;

struct PerChunkData {
    mat4 model;
};


layout(std430, binding = 0) restrict readonly buffer PerChunkSSBO {
    PerChunkData o[];
};

vec3 colors[] = {
  vec3(0.427f, 0.549f, 0.196f),
  vec3(1.0f, 0.957f, 0.878f),
  vec3(0.173f, 0.106f, 0.18f),
  vec3(0.271f, 0.486f, 0.839f),
};

vec3 normals[] = {
  vec3(0.0f, 0.0f, -1.0f),
  vec3(0.0f, 0.0f,  1.0f),
  vec3(-1.0f, 0.0f,  0.0f),
  vec3(1.0f, 0.0f,  0.0f),
  vec3(0.0f, -1.0f, 0.0f),
  vec3(0.0f,  1.0f, 0.0f),
};

uniform mat4 vp;

void main() {
  int x = int((data >> 24) & 0xFF);
  int y = int((data >> 16) & 0xFF);
  int z = int((data >> 8) & 0xFF);
  int n = int(data & 0xFF);
  mat4 model = o[gl_DrawID].model;
  FragPos = vec3(model * vec4(x, y, z, 1.0));
  Normal = mat3(transpose(inverse(model))) * normals[n];
  Color = colors[0];
  gl_Position = vp * vec4(FragPos, 1.0);
}
