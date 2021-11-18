#version 460
layout(location = 0) out vec4 fragColor;


in vec2 UV;
in float ZOOM;
in float ITERATIONS;

// Zo = 0
//Zn+1 = Z*Z + C !!! multiplicación de complejos !!!
// coloracion, qué tan rápido se alejan Zo


void main()
{
    if (ITERATIONS < 1) discard;
    vec2 C = (UV - 0.5) * 2;
    //C = step(0.0, C);
    //fragColor = vec4(C.x, C.y, 0, 1);
    float mandelbrot, f_length;
    float i = 0;
    vec2 Z = vec2(0, 0), Zsquare;
    while (i < ITERATIONS) 
    {
        //(ac - bd) + i(ad + bc)
        //c = a d = b
        Zsquare.x = Z.x * Z.x - Z.y * Z.y;
        Zsquare.y = Z.x * Z.y + Z.y * Z.x;
        Z = Zsquare + C;
        i++;
        if ((Z.x * Z.x + Z.y * Z.y) > 4) break;
    }
    f_length = Z.x * Z.x + Z.y * Z.y;
    mandelbrot = step(0.4, f_length) * i / ITERATIONS;
    fragColor = vec4(mandelbrot, mandelbrot, mandelbrot, 1);
}