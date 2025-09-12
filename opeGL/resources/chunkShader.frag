#version 460 core

struct Material {
  vec3 ambient;
  vec3 diff;
  vec3 spec;
  float shininess;
};

struct Light {
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;

uniform vec3 viewPos;
uniform Light light;

void main() {
  Material material;
  material.diff = Color * 0.5;
  material.ambient = material.diff * 0.2;
  material.spec = vec3(0.5); 
  material.shininess = 32.0;

  vec3 ambient = light.ambient * material.ambient;
  
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(-light.direction);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse * (diff * material.diff);

  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * (spec * material.spec);

  vec3 result = ambient + diffuse + specular;
  result = pow(result, vec3(1.0 / 2.2));
  FragColor = vec4(result, 1.0);
}
