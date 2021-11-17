#version 460
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform float appTime;

uniform vec3 lightSrc;

out vec4 LIGHTSRC;
out vec4 NORMAL;
out vec2 UV;
void main()
{
    //shd standard
    mat4 objectToWorld = projectionMatrix * viewMatrix * modelMatrix;
    vec4 pos = vec4(position, 1);
    vec4 norm = vec4(normal, 0);
    norm = modelMatrix * norm;
    vec4 worldPos = objectToWorld * pos;
    gl_Position = worldPos;
    //end shd standard

    vec4 light = normalize(vec4(lightSrc, 1) - pos);
    LIGHTSRC = light;
    NORMAL = norm;
    UV = uv;
}