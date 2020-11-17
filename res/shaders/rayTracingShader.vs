#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 3) in vec2 texCoords;

out vec4 vertexColor; // specify a color output to the fragment shader
out vec2 complex;

uniform mat4 MVP;
uniform mat4 Normal;

void main()
{
    complex = texCoords;
    gl_Position = MVP *Normal* vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
    vertexColor = vec4(0.5, 1.0, 0.5, 1.0); // set the output variable to a dark-red color
} 
