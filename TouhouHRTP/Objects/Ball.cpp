#include "Ball.h"

Ball::Ball(const uint32& screen_width, const uint32& screen_height)
{
	this->screen_width_	= screen_width;
	this->screen_height_ = screen_height;
	this->Ball::init_textures();
	this->temp_ = this->sprites_["ball"][this->iter_];
}

Ball::~Ball()
{
	for (auto& kv : this->sprites_)
		for (auto sprite : kv.second)
			delete sprite;
}

void Ball::fall(const float32& delta_time)
{
	this->iter_ %= this->sprites_["ball"].size();

	this->temp_ = this->sprites_["ball"][iter_];
	this->time_ += delta_time;

	this->time_ *= static_cast<float32>(this->time_ < (1000.0f / glm::clamp(fabs(this->y_vel_ * this->x_vel_), 5000.0f, 20000.0f)));
	
	this->iter_ -= signum(this->x_vel_) * (static_cast<int32>(this->time_ == 0.0f));
}

auto Ball::get_vel		() -> glm::vec2
{
	return {x_vel_, y_vel_};
}

void Ball::set_vel		(const float32& x_vel, const float32& y_vel)
{
	this->x_vel_ = x_vel;
	this->y_vel_ = y_vel;
}

void Ball::set_upper_bound(const float32& upper_bound)
{
	this->upper_b_ = upper_bound;
}

void Ball::reflectx()
{
	this->x_vel_ = -this->x_vel_;
}

void Ball::reflecty()
{
	this->y_vel_ = -this->y_vel_ * this->e_loss_;
}

void Ball::init_textures	()
{
	this->sprites_["ball"] = std::vector<Sprite*>
	{
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(97,  97, 122, 122), glm::ivec2(this->screen_width_, this->screen_height_)),
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(97, 123, 122, 148), glm::ivec2(this->screen_width_, this->screen_height_)),
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(97, 149, 122, 174), glm::ivec2(this->screen_width_, this->screen_height_)),
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(97, 175, 122, 199), glm::ivec2(this->screen_width_, this->screen_height_)) 
	};
}

void Ball::update(const float32& delta_time)
{
	this->fall(delta_time);
	this->set_pos(this->get_pos().x + this->x_vel_ * delta_time, this->get_pos().y + this->y_vel_ * delta_time);

	this->y_vel_ += this->gravity_ * delta_time;
	if (this->get_pos().y < this->get_size().y / 2.0f + 1.0f)
	{
		this->reflecty();
		this->set_pos(this->get_pos().x, this->get_size().y / 2.0f + 2.0f);
	}
	if (this->get_pos().y > (static_cast<float32>(this->screen_height_) - this->upper_b_ - this->get_size().x / 2.0f))
	{
		this->reflecty();
		this->set_pos(this->get_pos().x, this->get_pos().y - 1.0f);
	}
	if (this->get_pos().x > (static_cast<float32>(this->screen_width_) - this->get_size().x / 2.0f))
	{
		this->reflectx();
		this->set_pos(this->get_pos().x - 1.0f, this->get_pos().y);
	}
	if (this->get_pos().x < this->get_size().x / 2.0f)
	{
		this->reflectx();
		this->set_pos(this->get_pos().x + 1.0f, this->get_pos().y);
	}
}

void Ball::draw()
{
	this->temp_->draw(Resources::get_shader("sprite"), this->pos_);
}
