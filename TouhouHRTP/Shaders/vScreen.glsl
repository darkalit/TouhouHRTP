#version 450 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTex;

out vec2 TexCoords;

uniform bool shake;
uniform float time;

void main()
{
	gl_Position = vec4(aPos.xy, 0.0, 1.0);
	TexCoords = aTex;

	if (shake)
	{
		float strength = 0.01;
		gl_Position.x += cos(time * 25.0) * strength;
		gl_Position.y += cos(time * 30.0) * strength;
	}
}