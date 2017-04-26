#version 330 core

in vec4 vPosition;
in vec3 vNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 viewPerspective;
uniform mat3 normalMat;
uniform vec3 lightPos = vec3(-2, -5, -2);

out vec3 position;
out vec3 eye;
out vec3 light;
out vec3 normal;

void main() {

	gl_Position = viewPerspective * model * vPosition;
	position = (model * vPosition).xyz;

	eye = vec3(0,0,0) - (view * model * vPosition).xyz;

	light = (view * vec4(lightPos, 1)).xyz + eye;

	normal = (view * model * vec4(vNormal,0)).xyz;
}