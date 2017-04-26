#version 330 core

uniform vec4 colour;
out vec4 out_colour;

void main()
{
	out_colour = colour;
}