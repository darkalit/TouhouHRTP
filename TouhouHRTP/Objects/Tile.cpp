#include "Tile.h"

Tile::Tile	(Texture* texture, const uint32& screen_width, const uint32& screen_height, const State& state)
{
	this->screen_width_	 = screen_width;
	this->screen_height_ = screen_height;
	this->state_ = state;
	this->texture_ = texture;
	this->Tile::init_textures();
	this->temp_ = this->sprites_[std::to_string(state)][0];
}

Tile::~Tile	()
{
	for (auto& kv : this->sprites_)
		for (auto sprite : kv.second)
			delete sprite;
	//delete this->shader_;
}

auto Tile::flip			() -> bool
{
	return (flip_f_ = true);
}

auto Tile::get_state() -> State
{
	return static_cast<State>(state_);
}

void Tile::init_textures	()
{
	this->sprites_["0"] = std::vector<Sprite*>
	{
		new Sprite(this->texture_, glm::ivec4(96, 408, 126, 438), glm::ivec2(this->screen_width_, this->screen_height_))
	};
	this->sprites_["1"] = std::vector<Sprite*>
	{
		new Sprite(this->texture_, glm::ivec4(96, 408, 126, 438), glm::ivec2(this->screen_width_, this->screen_height_)),
		new Sprite(this->texture_, glm::ivec4(96, 438, 126, 468), glm::ivec2(this->screen_width_, this->screen_height_)),
		new Sprite(this->texture_, glm::ivec4(96, 468, 126, 498), glm::ivec2(this->screen_width_, this->screen_height_))
	};
	this->sprites_["2"] = std::vector<Sprite*>
	{
		new Sprite(this->texture_, glm::ivec4(96, 317, 126, 347), glm::ivec2(this->screen_width_, this->screen_height_)),
		new Sprite(this->texture_, glm::ivec4(96, 347, 126, 377), glm::ivec2(this->screen_width_, this->screen_height_)),
		new Sprite(this->texture_, glm::ivec4(96, 377, 126, 407), glm::ivec2(this->screen_width_, this->screen_height_))
	};
	this->sprites_["3"] = std::vector<Sprite*>
	{
		new Sprite(this->texture_, glm::ivec4(96, 226, 126, 256), glm::ivec2(this->screen_width_, this->screen_height_)),
		new Sprite(this->texture_, glm::ivec4(96, 256, 126, 286), glm::ivec2(this->screen_width_, this->screen_height_)),
		new Sprite(this->texture_, glm::ivec4(96, 286, 126, 316), glm::ivec2(this->screen_width_, this->screen_height_))
	};
}

void Tile::update		(const float32& delta_time)
{
	if (this->flip_f_) 
	{	
		if (this->iter_ >= this->sprites_[std::to_string(this->state_)].size())
		{
			if (this->state_ > 0)
			{
				--this->state_;
				this->flip_f_ = false;
			}
			this->iter_ = 0;
		}
		this->temp_ = this->sprites_[std::to_string(this->state_)][iter_];
		this->time_++;
		if (this->time_ * delta_time > 0.3f)
		{
			this->time_ = 0.0f;
			this->iter_++;
		}
		
	}	
}

void Tile::draw			(Shader* shader)
{
	this->temp_->draw(shader, this->pos_);
}