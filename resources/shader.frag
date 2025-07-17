#version 460 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

uniform vec3 viewPos;
uniform DirectionalLight dirLight;
uniform int numOfPointLights;
uniform PointLight pointLights[16];
uniform Material material;
uniform int tiling;
layout (location = 11) uniform sampler2D shadowMap;

vec3 calculateDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, float shadow) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords * tiling).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords * tiling).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords * tiling).rgb;
    return ambient + (1.0 - shadow) * (diffuse + specular);
//    return shadow == 1.0 ? vec3(1.0, 0.0, 0.0) : vec3(0.0, 0.0, 1.0); //for debuggin shadows, sigh
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance));
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords * tiling).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords * tiling).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords * tiling).rgb;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return ambient + (1.0 - shadow) * (diffuse + specular);
}

float calculateShadow(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
    return shadow;
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    float shadow = calculateShadow(FragPosLightSpace);

    vec3 result = calculateDirLight(dirLight, norm, viewDir, shadow);

    for (int i = 0; i < numOfPointLights; i++)
        //        result += calculatePointLight(pointLights[i], norm, FragPos, viewDir, shadow);

        result = pow(result, vec3(1.0 / 2.2));
    FragColor = vec4(result, 1.0);
}
