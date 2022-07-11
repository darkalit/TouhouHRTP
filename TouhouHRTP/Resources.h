#pragma once

#include "Render/Shader.h"
#include "Image/Texture.h"

class Resources
{
public:
	static Shader* load_shader	(const std::string& name, const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr, const bool& file_f = true);
	static Shader* get_shader	(const std::string& name);

	static Texture* load_texture	(const std::string& name, const std::string& path, const bool& bin_f = true);
	static Texture* get_texture		(const std::string& name);

	static void clear();
private:
	Resources() = default;

	static std::unordered_map<std::string, Shader*>	shaders_;
	static std::unordered_map<std::string, Texture*>	textures_;
};
