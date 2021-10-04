#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec4 color;
uniform sampler2D tex;

void main()
{    
    FragColor = texture(tex, TexCoords);
    //FragColor = vec4((color.rgb), 1.0f);
}