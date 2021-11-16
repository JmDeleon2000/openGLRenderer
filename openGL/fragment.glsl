#version 460
layout(location = 0) out vec4 fragColor;
in vec3 outColor;
in float appTimeToFrag;
in float worldY;
void main()
{
    fragColor = vec4(outColor, (1+cos(appTimeToFrag*2 + worldY*100))/2);
}