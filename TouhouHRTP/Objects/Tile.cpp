#include "Tile.h"

Tile::Tile	(const uint32& screen_width, const uint32& screen_height, const uint8& phase)
{
	this->screen_width_	 = screen_width;
	this->screen_height_ = screen_height;
	this->texture_.texFromImage("Textures/tiles.png");
	this->Tile::init_textures();
	this->temp_ = this->sprites_[std::to_string(phase)][0];
}

Tile::~Tile	()
{
	for (auto& kv : this->sprites_)
		for (auto sprite : kv.second)
			delete sprite;
}

auto Tile::flip			() -> bool
{
	return (flip_f_ = true);
}

auto Tile::get_state() -> State
{
	return state_;
}

void Tile::init_textures	()
{
	this->sprites_["1"] = std::vector<Sprite*>
	{
		new Sprite(this->texture_, glm::ivec4(101, 450, 131, 480), glm::ivec2(this->screen_width_, this->screen_height_)),
		new Sprite(this->texture_, glm::ivec4(101, 480, 131, 510), glm::ivec2(this->screen_width_, this->screen_height_)),
		new Sprite(this->texture_, glm::ivec4(101, 510, 131, 540), glm::ivec2(this->screen_width_, this->screen_height_))
	};
}

void Tile::update		(const float32& delta_time)
{
	if (this->flip_f_) 
	{	
		if (this->iter_ >= this->sprites_["1"].size())
		{
			this->state_ = State::DEAD;
			this->iter_ = 0;
		}
		this->temp_ = this->sprites_["1"][iter_];
		this->time_++;
		if (this->time_ * delta_time > 0.3f)
		{
			this->time_ = 0.0f;
			this->iter_++;
		}
		
	}	
}

void Tile::draw			()
{
	this->temp_->draw(this->pos_);
}
