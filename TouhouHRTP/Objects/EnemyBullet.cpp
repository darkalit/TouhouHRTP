#include "EnemyBullet.h"

EnemyBullet::EnemyBullet(const uint32& screen_width, const uint32& screen_height)
{
	this->screen_width_	 = screen_width;
	this->screen_height_ = screen_height;
	this->temp_ = Resources::get_sprites("EBullet")[0];
}

void EnemyBullet::set_direction(const glm::vec2& direction)
{
	this->direction_ = direction;
}

auto EnemyBullet::get_direction() -> glm::vec2
{
	return this->direction_;
}

void EnemyBullet::update(const float32& delta_time)
{
	this->pos_ += direction_ * delta_time * 300.0f;
}

void EnemyBullet::draw()
{
	this->temp_->draw(Resources::get_shader("sprite"), this->pos_);
}
