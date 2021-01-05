#version 330

in vec3 position;
in vec3 color;
in vec3 normal;
in vec2 texCoords;

out vec2 texCoord0;
out vec3 normal0;
out vec3 color0;
out vec3 position0;

uniform mat4 Proj;
uniform mat4 View;
uniform mat4 Model;

void main()
{
	
	texCoord0 = texCoords;
	color0 = color;
	normal0 = (Model  * vec4(normal, 0.0)).xyz;
	position0 = vec3(Model * vec4(position, 1.0));
	
	vec4 AmbientProduct = vec4(0.1,0.1,0.1,1);
	vec4 DiffuseProduct = vec4(0.7,0.7,0.7,1);
	vec4 SpecularProduct = vec4(0.8,0.8,0.8,1);
	vec4 LightPosition = vec4(2.0,4.0,8.0,0.7);
	float Shininess = 3.0;
	// Transform vertex position into eye coordinates 
	vec3 L = normalize(LightPosition.xyz - position0); 
	vec3 E = normalize(-position0);
	vec3 H = normalize(L + E);
	// Transform vertex normal into eye coordinates
	vec3 N = normalize(normal0);
	// Compute terms in the illumination equation 
	vec4 ambient = AmbientProduct;
	float Kd = max(dot(L, N), 0.0);
	vec4 diffuse = Kd*DiffuseProduct;
	float Ks = pow(max(dot(N, H), 0.0), Shininess); 
	vec4 specular = Ks * SpecularProduct;
	if( dot(L, N) < 0.0 )
		specular = vec4(0.0, 0.0, 0.0, 1.0);
	gl_Position = View * Model * vec4(position,1.0);
	color0 = (ambient + diffuse + specular).xyz;
	color0 = clamp(color0, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
}