#version 450 core
layout(location = 0) in vec2 aPos;

out vec2 Coords;

uniform mat4 model;
uniform mat4 projection;

void main()
{
	Coords = aPos;
	gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
}