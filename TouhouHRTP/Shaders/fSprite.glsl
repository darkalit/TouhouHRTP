#version 450 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
	vec4 res = texture(texture_diffuse1, TexCoords);
	if (res.a == 0.0)
		discard;
	FragColor = res;
}