#include "Rail.h"

Rail::Rail(const uint32& screen_width, const uint32& screen_height)
{
	this->screen_width_	 = screen_width;
	this->screen_height_ = screen_height;
	this->Rail::init_textures();
	this->temp_ = this->sprites_["0"][0];
}

Rail::~Rail()
{
	for (auto& kv : this->sprites_)
		for (auto sprite : kv.second)
			delete sprite;
}

void Rail::set_rot(const bool& vert)
{
	this->vertical_f_ = vert;
	this->temp_ = this->sprites_.at("0")[vert];
}

auto Rail::get_rot() -> bool
{
	return this->vertical_f_;
}

void Rail::draw()
{
	this->temp_->draw(Resources::get_shader("sprite"), 
					this->pos_,
					glm::vec2(1.0f, 1.0f),
					this->angle_);
}

void Rail::init_textures()
{
	this->sprites_["0"] = std::vector<Sprite*>
	{
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(96, 647, 128, 653), glm::ivec2(this->screen_width_, this->screen_height_)),
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(96, 679, 102, 711), glm::ivec2(this->screen_width_, this->screen_height_))
		//new Sprite(Resources::get_texture("tiles"), glm::ivec4(96, 736, 128, 768), glm::ivec2(this->screen_width_, this->screen_height_)),
		//new Sprite(Resources::get_texture("tiles"), glm::ivec4(96, 768, 128, 800), glm::ivec2(this->screen_width_, this->screen_height_))
	};
}
