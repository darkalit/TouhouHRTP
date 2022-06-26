#pragma once

#include "Shader.h"

class Resources
{
public:
	static std::map<std::string, Shader*> shaders;

	static Shader& loadShader	(bool fileFlag, std::string name, const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);
	static Shader& getShader	(std::string name);

private:
	Resources() {}
	static Shader& loadShaderFromFile(const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);
};
