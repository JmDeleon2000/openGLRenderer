#version 460
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform float appTime;

uniform vec3 lightSrc;

out float worldY;
out vec3 RAWNORMAL;
out vec2 UV;
void main()
{
    mat4 objectToWorld = projectionMatrix * viewMatrix * modelMatrix;
    float timeCos = cos(appTime);
    float timeSin = sin(appTime * 5);
    vec4 pos = vec4((sin(appTime * 15 * timeSin * timeCos) + position.x * 1.5 + sin(position.y * position.z * 3 * timeCos)) * step(0.80, timeSin)
        + position.x * (1 - step(0.95, timeSin)),
        position.y + timeCos / 2,
        position.z, 1.0);
    vec4 norm = vec4(normal, 0);
    vec4 worldPos = objectToWorld * pos;
    gl_Position = worldPos;
    worldY = worldPos.y;
    RAWNORMAL = normal;
    UV = uv;
}