#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec4 color;

void main()
{    
    //FragColor = texture(texture_diffuse1, TexCoords);
    FragColor = vec4((color.rgb), 1.0f);
}