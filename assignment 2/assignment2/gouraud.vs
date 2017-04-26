#version 330 core
/*
 *  Gouraud shader for assignment 2
 */

in vec4 vPosition;
in vec3 vNormal;

uniform mat4 modelView;
uniform mat4 projection;
uniform mat3 normalMat;

vec3 normal;
vec4 position;

uniform vec3 eye;

out vec4 LightingColour;

void main() {
	gl_Position = projection * modelView * vPosition;

	position = vPosition;
	normal = vNormal;

	vec3 N;
	vec3 Lposition = vec3(500.0, 500.0, 800.0);
	vec4 Lcolour = vec4(1.0, 1.0, 1.0, 1.0);
	vec3 R;
	float diffuse;
	float specular;
	float n = 100.0;
	vec3 L;

	N = normalize(normal);
	L = Lposition - position.xyz;
	L = normalize(L);
	
	R = reflect(-L, N);
	
	diffuse = dot(N,L);
	if(diffuse < 0.0) {
		diffuse = 0.0;
		specular = 0.0;
	} else {
		specular = pow(max(0.0, dot(R,normalize(eye))),n);
	}

	LightingColour = 0.3*Lcolour + diffuse*Lcolour + Lcolour*specular;
} 