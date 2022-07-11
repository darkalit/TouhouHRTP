#include "Ball_tp.h"

Ball_tp::Ball_tp(const uint32& screen_width, const uint32& screen_height)
{
	this->screen_width_	 = screen_width;
	this->screen_height_ = screen_height;
	this->Ball_tp::init_textures();
	this->temp_ = this->sprites_["0"][0];
}

Ball_tp::~Ball_tp()
{
	for (auto& kv : this->sprites_)
		for (auto sprite : kv.second)
			delete sprite;
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

void Ball_tp::init_textures()
{
	this->sprites_["0"] = std::vector<Sprite*>
	{
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(96, 776, 128, 808), glm::ivec2(this->screen_width_, this->screen_height_))
	};
}
