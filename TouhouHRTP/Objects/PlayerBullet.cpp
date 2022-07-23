#include "PlayerBullet.h"

PlayerBullet::PlayerBullet(const glm::vec2& pos, const uint32& screen_width, const uint32& screen_height)
{
	this->screen_width_ = screen_width;
	this->screen_height_ = screen_height;
	this->pos_ = pos;
	this->temp_ = Resources::get_sprites("PBullet")[0];
}

Sprite* PlayerBullet::get_temp()
{
	return this->temp_;
}

void PlayerBullet::update(const float32& deltaTime)
{
	this->set_pos(get_pos().x, get_pos().y + 600.0f * deltaTime);
}

void PlayerBullet::draw()
{
	this->temp_->draw(Resources::get_shader("sprite"), this->pos_);
}
