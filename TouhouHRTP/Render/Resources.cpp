#include "Resources.h"

std::map<std::string, Shader*> Resources::shaders;

Shader& Resources::loadShader(bool fileFlag, std::string name, const char* vertexSource, const char* fragmentSource, const char* geometrySource)
{
    if (fileFlag)
        shaders[name] = &loadShaderFromFile(vertexSource, fragmentSource, geometrySource);
    else
    {
        Shader shader;
        shader.compile(vertexSource, fragmentSource, geometrySource);
        shaders[name] = &shader;
    }
    return *shaders[name];
}

Shader& Resources::getShader(std::string name)
{
    return *shaders[name];
}

Shader& Resources::loadShaderFromFile(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
{
    std::string vShaderCode;
    std::string fShaderCode;
    std::string gShaderCode;
    
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
    	vShaderFile.open(vertexSource);
    	fShaderFile.open(fragmentSource);
    	std::stringstream vShaderStream, fShaderStream;
    
    	vShaderStream << vShaderFile.rdbuf();
    	fShaderStream << fShaderFile.rdbuf();
    	vShaderFile.close();
    	fShaderFile.close();
    
    	vShaderCode = vShaderStream.str();
    	fShaderCode = fShaderStream.str();
    
    	if (geometrySource != nullptr)
    	{
    		gShaderFile.open(geometrySource);
    		std::stringstream gShaderStream;
    		gShaderStream << gShaderFile.rdbuf();
    		gShaderFile.close();
    		gShaderCode = gShaderStream.str();
    	}
    }
    catch (std::ifstream::failure e)
    {
    	std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    const char* vertexCode = vShaderCode.c_str();
    const char* fragmentCode = fShaderCode.c_str();
    const char* geometryCode = gShaderCode.c_str();

    Shader shader;
    shader.compile(vertexCode, fragmentCode, geometrySource != nullptr ? geometryCode : nullptr);
    return shader;
}
