#version 330 core
out vec4 FragColor;
  
in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)  
in vec2 texCoord0;

uniform sampler2D sampler;
uniform vec4 ourColor;
uniform float k;
uniform int rgb;

void main()
{
    FragColor = ourColor * texture(sampler, texCoord0);// + 0.5 * ourColor;
    //FragColor.a = 0.1;
    //FragColor = ourColor;
    switch(rgb){
        case 0:
            FragColor.r += k;
            break;
        case 1:
            FragColor.g += k;
            break;
        case 2:
            FragColor.b += k;
            break;

    }

    //FragColor = vertexColor;//ourColor;//
} 
