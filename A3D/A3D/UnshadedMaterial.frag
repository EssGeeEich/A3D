#version 330 core

out vec4 fragColor;
in vec2 TexCoord;
uniform sampler2D AlbedoTexture;

void main()
{
    fragColor = texture(AlbedoTexture, TexCoord);
}
