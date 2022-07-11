#include "EnemyBullet.h"

EnemyBullet::EnemyBullet(const uint32& screen_width, const uint32& screen_height)
{
	this->screen_width_	 = screen_width;
	this->screen_height_ = screen_height;
	this->EnemyBullet::init_textures();
	this->temp_ = this->sprites_.at("0")[0];
}

EnemyBullet::~EnemyBullet()
{
	for (auto& kv : this->sprites_)
		for (auto sprite : kv.second)	
			delete sprite;
}

void EnemyBullet::set_direction(const glm::vec2& direction)
{
	this->direction_ = direction;
}

void EnemyBullet::update(const float32& delta_time)
{
	this->pos_ += direction_ * delta_time * 300.0f;
}

void EnemyBullet::draw()
{
	this->temp_->draw(Resources::get_shader("sprite"), this->pos_);
}

void EnemyBullet::init_textures()
{
	this->sprites_["0"] = std::vector<Sprite*>
	{
		new Sprite(Resources::get_texture("tiles"), glm::ivec4(97, 17, 105, 25), glm::ivec2(this->screen_width_, this->screen_height_))
	};
}
