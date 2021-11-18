
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform float appTime;

uniform float mandelbrotZoom;
//uniform float mandelbrotIterations;

out float ZOOM;
out vec2 UV;
out float ITERATIONS;
void main()
{
    //shd standard
    mat4 objectToWorld = projectionMatrix * viewMatrix * modelMatrix;
    vec4 pos = vec4(position * 2, 1);
    vec4 norm = vec4(normal, 0);
    norm = modelMatrix * norm;
    vec4 worldPos = objectToWorld * pos;
    gl_Position = worldPos;
    //end shd standard

    ZOOM = mandelbrotZoom;
    //ITERATIONS = mandelbrotIterations;
    //ZOOM = 0;
    ITERATIONS = 800;
    UV = uv;
}