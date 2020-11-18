#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 3) in vec2 texCoords;

out vec3 position;

uniform mat4 MVP;
uniform mat4 Normal;

void main()
{
    position = aPos;
    gl_Position = MVP *Normal* vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
} 
