#pragma once

#include "../headers.h"

class Shader
{
public:
	uint32 ID{};

	Shader	() { this->ID = 0; }
	Shader	(const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr, const bool& fileRead = false);
	//Shader	(const char* vertexSource)
	~Shader	();

	void compile	(const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);
	void compile_from_file (const char* vertexFile, const char* fragmentFile, const char* geometryFile = nullptr);
	Shader &use		();

	void setBool	(const std::string& name, const bool&		value);
	void setInt		(const std::string& name, const int32&		value);
	void setFloat	(const std::string& name, const float32&	value);
	void setDouble	(const std::string& name, const float64&	value);
	void setVec2	(const std::string& name, const glm::vec2&	value);
	void setVec3	(const std::string& name, const glm::vec3&	value);
	void setVec4	(const std::string& name, const glm::vec4&	value);
	void setMat4	(const std::string& name, const glm::mat4&	value);

private:
	void checkCompileErrors(const uint32& shader, const std::string& type);
};
