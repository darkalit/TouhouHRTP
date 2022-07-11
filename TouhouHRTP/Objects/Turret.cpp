#include "Turret.h"

Turret::Turret(const uint32& screen_width, const uint32& screen_height)
{
	this->screen_width_	 = screen_width;
	this->screen_height_ = screen_height;
	this->Turret::init_textures();
	this->temp_ = this->sprites_["0"][0];
}

Turret::~Turret()
{
	for (auto& kv : this->sprites_)
		for (auto sprite : kv.second)	
			delete sprite;
}

void Turret::set_bullets_count(const int32& count)
{
	this->bullets_count_ = count;
}

void Turret::shoot(const glm::vec2& target, std::vector<EnemyBullet*>& bullets)
{
	if (this->time_ > 2.0f)
	{
		constexpr float32 step_angle = glm::radians(15.0f);
		for (int32 i = 0; i < bullets_count_; ++i)
		{
			const float32 step = ceil(static_cast<float32>(2 * i - bullets_count_) * 0.5f) * step_angle
				+ static_cast<float32>(bullets_count_ % 2 == 0) * step_angle * 0.5f;
			const glm::vec2 direction = glm::normalize((target - this->pos_));
			bullets.push_back(new EnemyBullet(this->screen_width_, this->screen_height_));
			bullets.back()->set_pos(this->pos_.x, this->pos_.y);
			bullets.back()->set_direction(
				{direction.x * cos(step) - direction.y * sin(step), 
				direction.x * sin(step) + direction.y * cos(step)});
		}
		this->time_ = 0.0f;
		this->shoot_f_ = true;
	}
}

void Turret::update(const float32& delta_time)
{
	this->temp_ = this->sprites_.at("0")[this->shoot_f_];

	this->time_ += delta_time * static_cast<float32>(!this->shoot_f_);
	this->anim_time_ += delta_time * static_cast<float32>(this->shoot_f_);
	if (this->anim_time_ > 0.5f)
	{
		this->anim_time_ = 0.0f;
		this->shoot_f_ = false;
	}
}

void Turret::draw()
{
	this->temp_->draw(Resources::get_shader("sprite"), this->pos_);
}

void Turret::init_textures()
{
	this->sprites_["0"] = std::vector<Sprite*>
	{
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(97, 586, 127, 616), glm::ivec2(this->screen_width_, this->screen_height_)),
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(97, 616, 127, 646), glm::ivec2(this->screen_width_, this->screen_height_))
	};
}
