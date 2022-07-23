#include "Bonus.h"

Bonus::Bonus(const uint8& var, const uint32& screen_width, const uint32& screen_height)
{
	this->screen_width_	= screen_width;
	this->screen_height_ = screen_height;
	if (var == 0)
	{
		this->bomb_ex_ = true;
		this->temp_ = Resources::get_sprites("BombEx")[0];
	}
	else
		this->temp_ = Resources::get_sprites("Points")[0];
}

auto Bonus::is_bomb() -> bool
{
	return this->bomb_ex_;
}

void Bonus::update(const float32& delta_time)
{
	if(this->pos_.y < 20.0f && !this->jump_f)
		this->jump_f = true;

	if (this->jump_f && this->time_ < 0.2f)
	{
		this->pos_.y += sinf(delta_time) * 200.0f;
		this->time_ += delta_time;
	}
	else
		this->pos_.y -= delta_time * 200.0f;
}

void Bonus::draw()
{
	this->temp_->draw(Resources::get_shader("sprite"), this->pos_);
}
