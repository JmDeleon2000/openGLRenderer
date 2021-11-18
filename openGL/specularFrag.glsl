#version 460
layout(location = 0) out vec4 fragColor;

uniform sampler2D tex;

in float lightSRCINT;
in vec3 VIEWDIR;
in vec4 LIGHTSRC;
in vec4 NORMAL;
in vec2 UV;
void main()
{
    float intensity;
    float specular = 16;
    intensity = dot(NORMAL, LIGHTSRC) * lightSRCINT;

    vec3 reflection = reflect(vec3(LIGHTSRC.x, LIGHTSRC.y, LIGHTSRC.z),
        vec3(NORMAL.x, NORMAL.y, NORMAL.z));
    float R = dot(VIEWDIR, reflection);
    float specInt = max(0, lightSRCINT * pow(R, specular));

    fragColor = texture(tex, UV) * vec4(vec3(1, 1, 1) * (intensity + specInt), 1);
}