#version 330 core
out vec4 FragColor;
  
in vec4 vertexColor; 
in vec2 complex;

uniform sampler2D sampler;
uniform int p;

#define MAX_ITERATIONS 100.0
#define THRESHOLD 10.0f

vec2 pow(vec2 num, int power) {
    float x = num.x, y = num.y;
    for (int i = 1; i < power; i ++){
        float cx = num.x * x - num.y * y;
        float cy = num.x * y + num.y * x;
        num.x = cx;
        num.y = cy;
    }
    return num;
}

float vec2Abs(vec2 num){
    return sqrt((num.x * num.x) + (num.y * num.y));
}

void main()
{
    vec2 agg = vec2(0.0f, 0.0f);
    vec2 repositioned = vec2((complex.x) * 2 - 1, (complex.y) * 2 - 1);
    int i; //iterations count
    for(i = 0; i < MAX_ITERATIONS; i++){
        agg = pow(agg, p) + repositioned; //z^p + c
        if(vec2Abs(agg) > THRESHOLD) break;
    }
    float norm = i / MAX_ITERATIONS; //normalized iteration count
    FragColor = texture(sampler, vec2(norm, norm));
} 
