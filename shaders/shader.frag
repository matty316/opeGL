#version 460 core

#define MAX_TEXTURES 16
#define MAX_POINT_LIGHTS 16

layout (location = 0) in vec2 texCoord;
layout (location = 1) in flat uint textureIndex;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 fragPos;

out vec4 FragColor;

uniform sampler2D textures[MAX_TEXTURES]; 

struct PointLight {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float constant;
  float linear;
  float quadratic;
};

uniform uint numOfPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform vec3 viewPos;

vec3 BlinnPhong(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor);

const vec3 gamma = vec3(2.2);

void main() {
  vec3 norm = normalize(normal);
  vec3 color = texture(textures[textureIndex], texCoord).rgb;
  vec3 lighting = vec3(0.0);

  for (int i = 0; i < pointLights.length(); i++)
    lighting += BlinnPhong(norm, fragPos, pointLights[i].position, pointLights[i].diffuse);
  color *= lighting;

  //color = pow(color, 1/gamma);
  FragColor = vec4(color, 1.0);
}

vec3 BlinnPhong(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor) {
    vec3 ambient = lightColor * 0.1;
    // diffuse
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * lightColor;    
    // simple attenuation
    float max_distance = 1.5;
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / distance;
    
    diffuse *= attenuation;
    specular *= attenuation;
    ambient *= attenuation;
    
    return diffuse + ambient;
}

