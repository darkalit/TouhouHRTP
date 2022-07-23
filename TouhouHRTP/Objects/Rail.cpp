#include "Rail.h"

Rail::Rail(const uint32& screen_width, const uint32& screen_height)
{
	this->screen_width_	 = screen_width;
	this->screen_height_ = screen_height;
	this->temp_ = Resources::get_sprites("Rail")[0];
}

void Rail::set_rot(const bool& vert)
{
	this->vertical_f_ = vert;
	this->temp_ = Resources::get_sprites("Rail")[vert];
}

auto Rail::get_rot() -> bool
{
	return this->vertical_f_;
}

void Rail::draw()
{
	this->temp_->draw(Resources::get_shader("sprite"), 
					this->pos_,
					glm::vec2(1.0f, 1.0f),
					this->angle_);
}
