#version 410 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
    vec4 color = texture(skybox, TexCoords);
    color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
    FragColor = color;
}
