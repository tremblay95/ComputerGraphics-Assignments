#version 330 core
/*
 *  Blinn-Phong shader from example 6 for assignment 2
 */

in vec3 normal;
in vec4 position;
uniform vec3 eye;

void main() {
	vec3 N;
	vec3 Lposition = vec3(500.0, 500.0, 800.0);
	vec4 colour = vec4(1.0, 0.0, 0.0, 1.0);
	vec4 Lcolour = vec4(1.0, 1.0, 1.0, 1.0);
	vec3 H;
	float diffuse;
	float specular;
	float n = 100.0;
	vec3 L;

	N = normalize(normal);
	L = Lposition - position.xyz;
	L = normalize(L);

	H = normalize(L + eye);
	
	diffuse = dot(N,L);
	if(diffuse < 0.0) {
		diffuse = 0.0;
		specular = 0.0;
	} else {
		specular = pow(max(0.0, dot(N,H)),n);
	}

	gl_FragColor = min(0.3*colour + diffuse*colour*Lcolour + Lcolour*specular, vec4(1.0));
	gl_FragColor.a = colour.a;
}