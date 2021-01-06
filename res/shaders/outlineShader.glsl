#version 330

out vec4 FragColor;

in vec2 texCoord0;
in vec3 normal0;
in vec3 color0;
in vec3 position0;

uniform vec4 lightColor;
uniform sampler2D outline;
uniform vec4 lightDirection;


void main()
{
	FragColor = vec4(0.04, 0.7, 0.26, 0.5); //you must have gl_FragColor
}
