#version 330 core


uniform vec4 colour;
uniform mat4 model;
uniform mat4 view;
uniform vec3 lightPos;

in vec3 position;
in vec3 eye;
in vec3 light;
in vec3 normal;

out vec3 color;

void main() {

	vec3 lColour = vec3(1,1,1);
	float lPower = 25.0f;

	vec3 diffuse = colour.rgb;
	vec3 ambient = vec3(0.1,0.1,0.1) * diffuse;
	vec3 specular = vec3(0.3,0.3,0.3);

	float distance = length(lightPos - position);

	vec3 N;
	vec3 L;

	N = normalize(normal);
	L = normalize(light);
	float cosTheta = clamp(dot(N,L), 0, 1);
	vec3 E = normalize(eye);
	vec3 R = reflect(-L, N);

	float cosAlpha = clamp(dot(E, R), 0, 1);

	color = ambient + diffuse * lColour * lPower * cosTheta / (distance*distance) +
		specular * lColour * lPower * pow(cosAlpha,5) / (distance*distance);

	//gl_FragColor = min(0.3*colour + 0.7*diffuse*colour, vec4(1.0));
	//gl_FragColor.a = colour.a;

}