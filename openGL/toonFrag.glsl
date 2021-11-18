#version 460
layout(location = 0) out vec4 fragColor;

uniform sampler2D tex;

in vec4 LIGHTSRC;
in vec4 NORMAL;
in vec2 UV;
void main()
{
    float intensity;
    intensity = dot(NORMAL, LIGHTSRC);
    intensity = 0.1 + step(0.25, intensity) * 0.25 + step(0.7, intensity) * 0.5;
    fragColor = texture(tex, UV) * vec4(vec3(1, 1, 1) * intensity, 1);
}