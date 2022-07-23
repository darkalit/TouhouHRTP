#include "Turret.h"

Turret::Turret(const uint32& screen_width, const uint32& screen_height, const uint8& bullets_count)
{
	this->screen_width_	 = screen_width;
	this->screen_height_ = screen_height;
	this->bullets_count_ = bullets_count;
	this->temp_ = Resources::get_sprites("Turret")[0];
}

void Turret::set_delay(const float32& delay)
{

}

void Turret::set_bullets_count(const uint8& count)
{
	this->bullets_count_ = count;
}

void Turret::shoot(const glm::vec2& target, std::vector<EnemyBullet*>& bullets)
{
	if (this->time_ > this->sh_delay_)
	{
		constexpr float32 step_angle = glm::radians(15.0f);
		for (uint8 i = 0; i < bullets_count_; ++i)
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

void Turret::reset()
{
	this->time_ = 0.0f;
	this->anim_time_ = 0.0f;
	this->shoot_f_ = false;
	this->iter_ = 0;
}

void Turret::update(const float32& delta_time)
{
	this->temp_ = Resources::get_sprites("Turret")[this->shoot_f_];

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
