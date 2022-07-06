#include "PlayerBullet.h"

PlayerBullet::PlayerBullet(const glm::vec2& pos, const uint32& screen_width, const uint32& screen_height)
{
	this->screen_width_ = screen_width;
	this->screen_height_ = screen_height;
	this->pos_ = pos;
	this->PlayerBullet::init_textures();
	this->temp_ = this->sprites_.at("Bullet")[0];
}

PlayerBullet::~PlayerBullet()
{
	for (auto& kv : this->sprites_)
		for (auto sprite : kv.second)
			delete sprite;
}

void PlayerBullet::update(const float32& deltaTime)
{
	this->set_pos(get_pos().x, get_pos().y + 600.0f * deltaTime);
}

void PlayerBullet::draw()
{
	this->temp_->draw(Resources::get_shader("sprite"), this->pos_);
}

void PlayerBullet::init_textures()
{
	sprites_["Bullet"] = 
	{
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(99, 32, 109, 48),	glm::ivec2(screen_width_, screen_height_))
	};
}
