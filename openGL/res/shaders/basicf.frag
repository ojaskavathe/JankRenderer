#version 460 core
out vec4 FragColor;

uniform vec3 fcolor = vec3(.3f);

void main()
{    
    //FragColor = texture(texture_diffuse1, TexCoords);
    FragColor = vec4(fcolor, 1.0f);
}