#version 460
layout(location = 0) out vec4 fragColor;

uniform sampler2D tex;

in vec3 RAWNORMAL;
in vec2 UV;
in float worldY;
void main()
{
    fragColor = vec4(RAWNORMAL, sin(worldY * 30) * 0.8);
}