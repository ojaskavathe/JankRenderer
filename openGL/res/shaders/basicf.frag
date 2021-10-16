#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec3 color;

void main()
{    
    //FragColor = texture(texture_diffuse1, TexCoords);
    if(gl_FrontFacing)
        FragColor = vec4(color, 1.0f);
}