#include "Bumper.h"

Bumper::Bumper(const uint32& screen_width, const uint32& screen_height)
{
	this->screen_width_	 = screen_width;
	this->screen_height_ = screen_height;
	this->Bumper::init_textures();
	this->temp_ = this->sprites_["0"][0];
}

Bumper::~Bumper()
{
	for (auto& kv : this->sprites_)
		for (auto sprite : kv.second)
			delete sprite;
}

void Bumper::draw()
{
	this->temp_->draw(Resources::get_shader("sprite"), this->pos_);
}

void Bumper::init_textures()
{
	this->sprites_["0"] = std::vector<Sprite*>
	{
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(101, 555, 125, 579), glm::ivec2(this->screen_width_, this->screen_height_))
	};
}
