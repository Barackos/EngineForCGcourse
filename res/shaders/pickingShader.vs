#version 330

in vec3 position;
in vec3 color;
in vec3 normal;
in vec2 texCoords;


uniform mat4 Proj;
uniform mat4 View;
uniform mat4 Model;
uniform mat4 Obj;

out vec3 normal0;

void main()
{
	normal0 = vec3(Obj * vec4(normal, 0.0));
	gl_Position = /*Proj */View * Model* vec4(position, 1.0);
}