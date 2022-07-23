#include "Tile.h"

Tile::Tile	(const uint32& screen_width, const uint32& screen_height, const uint8& hp, const uint8& skin)
{
	this->screen_width_	 = screen_width;
	this->screen_height_ = screen_height;
	this->initial_hp_ = hp;
	this->skin_ = skin;
	//this->state_ = state;
	this->hp = hp;
	this->temp_ = Resources::get_sprites("T" + std::to_string(this->skin_))[0];
}

auto Tile::flip			() -> bool
{
	if (this->play_s_f)
		this->play_s_f = false;
	return (this->flip_f_ = true);
}

auto Tile::get_starting() -> bool
{
	return this->start_f_;
}

auto Tile::get_hp_change() -> bool
{
	return this->hp_change_;
}

void Tile::reset()
{
	this->hp = this->initial_hp_;
	this->iter_ = Resources::get_sprites("T" + std::to_string(this->skin_)).size() - 1;
	this->flip_f_ = false;
	this->start_f_ = true;
	this->hp_change_ = false;
	this->temp_ = Resources::get_sprites("T" + std::to_string(this->skin_))[iter_];
}

void Tile::update		(const float32& delta_time)
{
	this->temp_hp_ = this->hp;
	this->hp_change_ = false;	

	if (this->flip_f_) 
	{
		this->hp -= this->hp > 0 && this->iter_ >= Resources::get_sprites("T" + std::to_string(this->skin_)).size();
		this->flip_f_ = (this->hp > 0) && (this->iter_ < Resources::get_sprites("T" + std::to_string(this->skin_)).size());
		this->iter_ *= this->iter_ < Resources::get_sprites("T" + std::to_string(this->skin_)).size();

		if (this->temp_hp_ != this->hp)
			this->hp_change_ = true;

		this->temp_ = Resources::get_sprites("T" + std::to_string(this->skin_))[iter_];
		this->time_++;

		this->time_ *= static_cast<float32>(this->time_ * delta_time <= 0.2f);
		this->iter_ += static_cast<uint32>(this->time_) == 0;

		if (this->hp == 0)
			this->temp_ = Resources::get_sprites("T0")[0];
	}
	if (this->start_f_)
	{
		this->start_f_ = (this->hp > 0) && (this->iter_ > 0);
		this->iter_ *= this->iter_ > 0;

		this->temp_ = Resources::get_sprites("T" + std::to_string(this->skin_))[iter_];
		this->time_++;

		this->time_ *= static_cast<float32>(this->time_ * delta_time <= 0.2f);
		this->iter_ -= static_cast<uint32>(this->time_) == 0;		
	}
}

void Tile::draw()
{
	this->temp_->draw(Resources::get_shader("sprite"), this->pos_);
}
