#include "Shader.h"

Shader::Shader(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
{
	this->compile(vertexSource, fragmentSource, geometrySource);
}

Shader::~Shader()
{
	glDeleteProgram(this->ID);
}

void Shader::compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
{
	uint32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	checkCompileErrors(vertexShader, "VERTEX");

	uint32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);	
	checkCompileErrors(fragmentShader, "FRAGMENT");

	uint32 geometryShader;
	if (geometrySource != nullptr)
	{
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryShader, 1, &geometrySource, NULL);
		glCompileShader(geometryShader);
		checkCompileErrors(geometryShader, "GEOMETRY");
	}	

	this->ID = glCreateProgram();
	glAttachShader(this->ID, vertexShader);
	glAttachShader(this->ID, fragmentShader);

	if (geometrySource != nullptr)
		glAttachShader(this->ID, geometryShader);

	glLinkProgram(this->ID);
	checkCompileErrors(this->ID, "PROGRAM");

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if (geometrySource != nullptr)
		glDeleteShader(geometryShader);
}

Shader &Shader::use		()
{
	glUseProgram(this->ID);
	return *this;
}

void Shader::setBool	(const std::string& name, const bool&		value)
{
	glUniform1i(glGetUniformLocation(this->ID, name.c_str()), value);
}

void Shader::setInt		(const std::string& name, const int32&		value)
{
	glUniform1i(glGetUniformLocation(this->ID, name.c_str()), value);
}

void Shader::setFloat	(const std::string& name, const float32&	value)
{
	glUniform1f(glGetUniformLocation(this->ID, name.c_str()), value);
}

void Shader::setDouble	(const std::string& name, const float64&	value)
{
	glUniform1d(glGetUniformLocation(this->ID, name.c_str()), value);
}

void Shader::setVec2	(const std::string& name, const glm::vec2&	value)
{
	glUniform2fv(glGetUniformLocation(this->ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec3	(const std::string& name, const glm::vec3&	value)
{
	glUniform3fv(glGetUniformLocation(this->ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec4	(const std::string& name, const glm::vec4&	value)
{
	glUniform4fv(glGetUniformLocation(this->ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setMat4	(const std::string& name, const glm::mat4&	value)
{
	glUniformMatrix4fv(glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::checkCompileErrors(const uint32& shader, const std::string& type)
{
	int32 success;
	char infoLog[1024];
	
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n" <<
				infoLog << std::endl;
		}
	}
	else
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" <<
				infoLog << std::endl;
		}
	}
}
