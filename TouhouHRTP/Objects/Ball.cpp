#include "Ball.h"

Ball::Ball(const uint32& screen_width, const uint32& screen_height)
{
	this->screen_width_	= screen_width;
	this->screen_height_ = screen_height;
	this->texture_.texFromImage("Textures/tiles.png");
	this->Ball::init_textures();
	this->temp_ = this->sprites_["ball"][this->iter_];
}

Ball::~Ball()
{
	for (auto& kv : this->sprites_)
		for (auto sprite : kv.second)
			delete sprite;
}

auto Ball::fall(const float32& delta_time) -> bool
{
	this->iter_ %= this->sprites_["ball"].size();

	this->temp_ = this->sprites_["ball"][iter_];
	this->time_++;

	this->time_ *= static_cast<float32>(this->time_ * delta_time < 1.0f / sqrt(fabs(this->y_vel_)));
	this->iter_ -= signum(this->x_vel_) * static_cast<int32>(this->time_) == 0;

	return true;
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

void Ball::init_textures	()
{
	this->sprites_["ball"] = std::vector<Sprite*>
	{
		new Sprite(this->texture_, glm::ivec4(102, 100, 128, 126), glm::ivec2(screen_width_, screen_height_)),
		new Sprite(this->texture_, glm::ivec4(102, 127, 128, 153), glm::ivec2(screen_width_, screen_height_)),
		new Sprite(this->texture_, glm::ivec4(102, 154, 128, 180), glm::ivec2(screen_width_, screen_height_)),
		new Sprite(this->texture_, glm::ivec4(102, 181, 128, 207), glm::ivec2(screen_width_, screen_height_)) 
	};
}

void Ball::update(const float32& delta_time)
{
	this->fall(delta_time);
	this->set_pos(this->get_pos().x + this->x_vel_ * delta_time, this->get_pos().y + this->y_vel_ * delta_time);

	this->y_vel_ += this->gravity_ * delta_time;
	if (this->get_pos().y < this->get_size().y + 1.0f)
	{
		this->y_vel_ = -this->y_vel_ * this->e_loss_;
		this->set_pos(this->get_pos().x, this->get_size().y + 2.0f);
	}
	if (this->get_pos().y > (static_cast<float32>(this->screen_height_) - this->get_size().x))
	{
		this->y_vel_ = -this->y_vel_ * this->e_loss_;
		this->set_pos(this->get_pos().x, this->get_pos().y - 1.0f);
	}
	if (this->get_pos().x > (static_cast<float32>(this->screen_width_) - this->get_size().x))
	{
		this->x_vel_ = -this->x_vel_;
		this->set_pos(this->get_pos().x - 1.0f, this->get_pos().y);
	}
	if (this->get_pos().x < this->get_size().x)
	{
		this->x_vel_ = -this->x_vel_;
		this->set_pos(this->get_pos().x + 1.0f, this->get_pos().y);
	}
}

void Ball::draw()
{
	this->temp_->draw(this->pos_);
}
