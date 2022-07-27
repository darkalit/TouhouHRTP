#version 450 core
layout(location = 0) in vec4 aPosTex;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;

void main()
{
	TexCoords = aPosTex.zw;
	gl_Position = projection * model * vec4(aPosTex.xy, 0.0, 1.0);
}