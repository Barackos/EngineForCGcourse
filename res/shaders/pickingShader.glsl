#version 330

out vec4 FragColor;

in vec3 normal0;

uniform sampler2D sampler1;
uniform vec4 lightDirection;
uniform vec4 lightColor;

void main()
{
	FragColor = vec4(lightColor.rgb * abs(normal0),lightColor.a);
}