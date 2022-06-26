#include "ShaderList.h"

const char* glsl::vSprite =
	"#version 450 core\n"
	"layout(location = 0) in vec4 aPosTex;\n"
	"out vec2 TexCoords;\n"
	"uniform mat4 model;\n"
	"uniform mat4 projection;\n"
	"void main()\n"
	"{\n"
	"	TexCoords = aPosTex.zw;\n"
	"	gl_Position = projection * model * vec4(aPosTex.xy, 0.0, 1.0);\n"
	"}\n\0";

const char* glsl::fSprite =
	"#version 450 core\n"
	"out vec4 FragColor;\n"
	"in vec2 TexCoords;\n"
	"uniform sampler2D texture_diffuse1;\n"
	"void main()\n"
	"{\n"
	"	vec4 res = texture(texture_diffuse1, TexCoords);\n"
	"	if (res.a == 0.0)\n"
	"		discard;\n"
	"	FragColor = res;\n"
	"}\n\0";


const char* glsl::vScreen =
	"#version 450 core\n"
	"layout(location = 0) in vec2 aPos;\n"
	"layout(location = 1) in vec2 aTex;\n"
	"out vec2 TexCoords;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
	"	TexCoords = aTex;\n"
	"}\n\0";

const char* glsl::fScreen =
	"#version 450 core\n"
	"out vec4 FragColor;\n"
	"in vec2 TexCoords;\n"
	"uniform sampler2D screenTexture;\n"
	"void main()\n"
	"{\n"
	"	FragColor = texture(screenTexture, TexCoords);\n"
	"}\n\0";


const char* glsl::vText =
	"#version 450 core\n"
	"layout (location = 0) in vec4 vertex;\n"
	"out vec2 TexCoords;\n"
	"uniform mat4 projection;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = projection * vec4(vertex.xy, 0.0, 1.0) * vec4(1.0, -1.0, 1.0, 1.0);\n"
	"	TexCoords = vertex.zw;\n"
	"}\n\0";

const char* glsl::fText =
	"#version 450 core\n"
	"in vec2 TexCoords;\n"
	"out vec4 color;\n"
	"uniform sampler2D text;\n"
	"uniform vec3 textColor;\n"
	"void main()\n"
	"{\n"
	"	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
	"	color = vec4(textColor, 1.0) * sampled;\n"
	"}\n\0";