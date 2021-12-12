#version 460 core
out vec4 FragColor;

in vec3 fColor;

void main()
{    
    //FragColor = texture(texture_diffuse1, TexCoords);
    FragColor = vec4(fColor, 1.0f);
}