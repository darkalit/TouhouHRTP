#include "Ball_tp.h"

Ball_tp::Ball_tp(const uint32& screen_width, const uint32& screen_height)
{
	this->screen_width_	 = screen_width;
	this->screen_height_ = screen_height;
	this->temp_ = Resources::get_sprites("Ball_TP")[0];
}

auto Ball_tp::tp_ready() -> bool
{
	return this->tp_flag_;
}

void Ball_tp::teleport(Object& object, Ball_tp& ball_tp)
{
	object.set_pos(ball_tp.get_pos().x, ball_tp.get_pos().y);
	ball_tp.tp_flag_ = false;
}

void Ball_tp::update(const float32& delta_time)
{
	if (this->anim_time_ > 1.0f && !this->tp_flag_)
	{
		this->tp_flag_ = true;
		this->anim_time_ = 0.0f;
	}
	if (!this->tp_flag_)
		this->anim_time_ += delta_time;
}

void Ball_tp::draw()
{
	this->temp_->draw(Resources::get_shader("sprite"), this->pos_);
}
