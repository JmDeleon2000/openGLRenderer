#version 460
layout(location = 0) out vec4 fragColor;
in vec3 outColor;
void main()
{
    fragColor = vec4(outColor, 1);
}