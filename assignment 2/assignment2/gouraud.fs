#version 330 core

in vec4 LightingColour;
vec4 colour = vec4(1.0, 0.0, 0.0, 1.0);

void main()
{
	gl_FragColor = min(LightingColour * colour, vec4(1.0));
	gl_FragColor.a = colour.a;
}