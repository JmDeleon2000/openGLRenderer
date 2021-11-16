#version 460
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 inColor;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform float appTime;

out vec3 outColor;
out float appTimeToFrag;
out float worldY;
void main()
//{
//    vec4 pos = vec4(position.x * sin(appTime) + position.y * cos(appTime), 
//        position.z * cos(appTime) + position.y * sin(appTime), 
//        position.z, 1.0);
//    gl_Position = projectionMatrix * viewMatrix * modelMatrix * pos;
//    outColor = inColor;
//}
{
    float timeCos = cos(appTime);
    float timeSin = sin(appTime);
    vec4 pos = vec4(position.x * (1.5+timeSin) + position.y * step(0.25, timeCos),
        position.z * (1.5+timeCos) + position.y * step(0.25, timeSin) + position.z * position.x * step(0.98, cos(appTime*10)) * 2,
        position.z, 1.0);
    vec4 worldPos = projectionMatrix * viewMatrix * modelMatrix * pos;
    gl_Position = worldPos;
    outColor = inColor;
    appTimeToFrag = appTime;
    worldY = worldPos.y;
}