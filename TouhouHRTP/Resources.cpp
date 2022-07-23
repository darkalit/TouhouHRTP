#include "Resources.h"

std::unordered_map<std::string, Shader*> Resources::shaders_;
std::unordered_map<std::string, Texture*> Resources::textures_;
std::unordered_map<std::string, std::vector<Sprite*>> Resources::sprites_;

Shader* Resources::load_shader(const std::string& name, const char* vertexSource, const char* fragmentSource, const char* geometrySource, const bool& file_f)
{
    shaders_[name] = new Shader(vertexSource, fragmentSource, geometrySource, file_f);
    return shaders_.at(name);
}

Shader* Resources::get_shader(const std::string& name)
{
    return shaders_.at(name);
}

Texture* Resources::load_texture(const std::string& name, const std::string& path, const bool& bin_f)
{
    textures_[name] = new Texture;
    if (bin_f)
		textures_.at(name)->texFromBin(path);
    else
		textures_.at(name)->texFromImage(path);
    return textures_.at(name);
}

Texture* Resources::get_texture(const std::string& name)
{
    return textures_.at(name);
}

Sprite* Resources::load_sprite(const std::string& name, Sprite* sprite)
{
    sprites_[name].push_back(sprite);
    return sprites_.at(name).back();
}

void Resources::load_sprites(const std::string& name, const std::vector<Sprite*>& sprites)
{
    sprites_[name] = sprites;
}

std::vector<Sprite*> Resources::get_sprites(const std::string& name)
{
    return sprites_.at(name);
}

void Resources::clear()
{
    for (auto& shader : shaders_)
		delete shader.second;

    for (auto& texture : textures_)
		delete texture.second;

    for (auto& sprite : sprites_)
        for (auto s : sprite.second)
            delete s;
}
