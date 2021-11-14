#version 460
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 inColor;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 outColor;
void main()
{
    vec4 pos = vec4(position.x, position.y, position.z, 1.0);
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * pos;
    outColor = inColor;
}