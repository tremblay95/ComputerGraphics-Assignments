#version 330 core
/*
 *  Blinn-Phong shader from example 6 for assignment 2
 */

in vec4 vPosition;
in vec3 vNormal;
uniform mat4 modelView;
uniform mat4 projection;
uniform mat3 normalMat;
out vec3 normal;
out vec4 position;

void main() {

	gl_Position = projection * modelView * vPosition;
	position = vPosition;
	normal = vNormal;
} 