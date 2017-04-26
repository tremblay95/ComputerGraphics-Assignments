#version 330 core

in vec4 position;

uniform mat4 model;
uniform mat4 viewPerspective;

void main()
{
	gl_Position = viewPerspective * model * position;
}