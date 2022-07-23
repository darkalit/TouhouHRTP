#include "Bumper.h"

Bumper::Bumper(const uint32& screen_width, const uint32& screen_height)
{
	this->screen_width_	 = screen_width;
	this->screen_height_ = screen_height;
	this->temp_ = Resources::get_sprites("Bumper")[0];
}

void Bumper::draw()
{
	this->temp_->draw(Resources::get_shader("sprite"), this->pos_);
}
