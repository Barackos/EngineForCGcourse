#version 330

out vec4 FragColor;

in vec2 texCoord0;
in vec3 normal0;
in vec3 color0;
in vec3 position0;

uniform vec4 lightColor;
uniform sampler2D sampler;
uniform vec4 lightDirection;


void main()
{
	FragColor = texture(sampler, texCoord0)* vec4(color0,1.0); //you must have gl_FragColor
}