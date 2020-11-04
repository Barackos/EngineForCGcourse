#version 330 core
out vec4 FragColor;
  
in vec4 vertexColor; 
in vec2 complex;

uniform sampler2D sampler;
uniform int p;

#define MAX_ITERATIONS 10
#define THRESHOLD 20.0f

vec2 pow(vec2 num, int power) {
    float x = num.x, y = num.y;
    for (int i = 1; i < power; i ++){
        num.x = x * x - y * y;
        num.y = 2 * (x * y);
    }
    return num;
}

float vec2Abs(vec2 num){
    return sqrt(num.x * num.x + num.y * num.y);
}

void main()
{
    vec2 agg = vec2(0.0f, 0.0f);
    int i; //iterations count
    for(i = 0; i < MAX_ITERATIONS; i++){
        agg = pow(agg, 2) + complex; //z^p + c
        if(vec2Abs(agg) > THRESHOLD) break;
    }
    float norm = i / MAX_ITERATIONS; //normalized iteration count
    if (i == 0) //debugging
        FragColor = texture(sampler, vec2(norm, norm));
    else
        FragColor = texture(sampler, complex);
} 
