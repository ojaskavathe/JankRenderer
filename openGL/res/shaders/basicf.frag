#version 460 core
out vec4 FragColor;

in vec3 fColor;

uniform vec3 color;

void main()
{    
    //FragColor = texture(texture_diffuse1, TexCoords);
    FragColor = vec4(color, 1.0f);
}