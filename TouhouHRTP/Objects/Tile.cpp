#include "Tile.h"

Tile::Tile	(const uint32& screen_width, const uint32& screen_height, const uint8& state)
{
	this->screen_width_	 = screen_width;
	this->screen_height_ = screen_height;
	this->state_ = state;
	this->Tile::init_textures();
	this->temp_ = this->sprites_[std::to_string(state)][0];
}

Tile::~Tile	()
{
	for (auto& kv : this->sprites_)
		for (auto sprite : kv.second)
			delete sprite;
}

auto Tile::flip			() -> bool
{
	return (this->flip_f_ = true);
}

auto Tile::get_state() -> State
{
	return static_cast<State>(this->state_);
}

auto Tile::get_state_change() -> bool
{
	return this->state_change_;
}

void Tile::init_textures	()
{
	this->sprites_["0"] = std::vector<Sprite*>
	{
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(96, 519, 128, 551), glm::ivec2(this->screen_width_, this->screen_height_))
	};
	this->sprites_["1"] = std::vector<Sprite*>
	{
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(97, 423, 129, 455), glm::ivec2(this->screen_width_, this->screen_height_)),
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(97, 455, 129, 487), glm::ivec2(this->screen_width_, this->screen_height_)),
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(97, 486, 129, 518), glm::ivec2(this->screen_width_, this->screen_height_))
	};
	this->sprites_["2"] = std::vector<Sprite*>
	{
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(97, 328, 129, 360), glm::ivec2(this->screen_width_, this->screen_height_)),
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(97, 360, 129, 392), glm::ivec2(this->screen_width_, this->screen_height_)),
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(97, 391, 129, 423), glm::ivec2(this->screen_width_, this->screen_height_))
	};
	this->sprites_["3"] = std::vector<Sprite*>
	{
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(97, 233, 129, 265), glm::ivec2(this->screen_width_, this->screen_height_)),
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(97, 265, 129, 297), glm::ivec2(this->screen_width_, this->screen_height_)),
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(97, 296, 129, 328), glm::ivec2(this->screen_width_, this->screen_height_))
	};
}

void Tile::update		(const float32& delta_time)
{
	this->temp_state_ = this->state_;
	this->state_change_ = false;
	if (this->flip_f_) 
	{
		this->state_ -= this->state_ > 0 && this->iter_ >= this->sprites_.at(std::to_string(this->state_)).size();
		this->flip_f_ = (this->state_ > 0) && (this->iter_ < this->sprites_.at(std::to_string(this->state_)).size());
		this->iter_ *= this->iter_ < this->sprites_.at(std::to_string(this->state_)).size();

		if (this->temp_state_ != this->state_)
			this->state_change_ = true;

		this->temp_ = this->sprites_[std::to_string(this->state_)][iter_];
		this->time_++;

		this->time_ *= static_cast<float32>(this->time_ * delta_time <= 0.3f);
		this->iter_ += static_cast<uint32>(this->time_) == 0;		
	}	
}

void Tile::draw()
{
	this->temp_->draw(Resources::get_shader("sprite"), this->pos_);
}
