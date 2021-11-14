#version 460
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 inColor;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform float appTime;

out vec3 outColor;
void main()
{
    vec4 pos = vec4(position.x * (2 + sin(appTime)), position.y, position.z * (2 + sin(appTime)), 1.0);
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * pos;
    outColor = inColor;
}