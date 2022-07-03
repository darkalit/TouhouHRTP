#include "Player.h"


Player::Player(Texture* texture, const uint32& screen_width, const uint32& screen_height)
{
	this->screen_width_ = screen_width;
	this->screen_height_ = screen_height;
	this->texture_ = texture;
	this->Player::init_textures();
	this->temp_ = this->sprites_["stand"][0];
}

Player::~Player()
{
	for (auto& kv : this->sprites_)
		for (auto sprite : kv.second)
			delete sprite;
}

auto Player::stand	() -> bool
{
	this->state_ = State::STAND;
	this->temp_ = this->sprites_["stand"][0];
	this->set_pos(this->get_pos().x, this->get_pos().y);
	this->set_scale(glm::vec2(1.0f, 1.0f));
	this->anim_time_ = 0.0f;
	return true;
}

auto Player::run	(const float32& delta_time, const int8& direction) -> bool
{
	this->state_ = State::RUN_L * (direction == -1) + State::RUN_R * (direction == 1);
	this->iter_ %= this->sprites_["run"].size();

	this->temp_ = this->sprites_["run"][iter_];
	this->time_ += delta_time;
	
	this->time_ *= static_cast<float32>(this->time_ < 0.1f);
	this->iter_ += this->time_ == 0.0f;

	this->set_pos(this->get_pos().x + static_cast<float32>(direction) * this->speed_ * delta_time, floor(this->get_pos().y));
	this->set_scale(glm::vec2(static_cast<float32>(-direction), 1.0f));

	this->anim_time_ = 0.0f;

	return true;
}

auto Player::attack1(const float32& delta_time) -> bool
{
	constexpr float32 speed_time = 0.5f;

	this->anim_time_ = speed_time * static_cast<float32>(anim_time_ <= 0.0f) + anim_time_;
	this->state_ = Player::State::ATTACK1;

	if (this->iter_ == this->sprites_["attack1"].size())
		this->flag_ = !this->flag_;

	this->iter_ %= this->sprites_["attack1"].size();

	this->temp_ = this->sprites_["attack1"][this->iter_];
	this->time_ += delta_time;
	
	this->time_ *= static_cast<float32>(this->time_ <= speed_time / static_cast<float32>(2 * this->sprites_["attack1"].size()));
	this->iter_ += this->time_ == 0.0f;

	this->set_scale(glm::vec2(static_cast<float32>(-!this->flag_ + this->flag_), 1.0f));

	this->anim_time_ -= delta_time;

	return true;
}

auto Player::slide	(const float32& delta_time, const int8& direction) -> bool
{
	this->anim_time_ = 0.2f * static_cast<float32>(anim_time_ <= 0.0f) + anim_time_;

	this->state_ = State::SLIDE_L * (direction == -1) + State::SLIDE_R * (direction == 1);
	this->iter_ %= this->sprites_["slide"].size();

	this->temp_ = this->sprites_["slide"][iter_];
	this->time_ += delta_time;

	this->time_ *= static_cast<float32>(this->time_ < 0.1f);
	this->iter_ += this->time_ == 0.0f;

	this->set_pos(this->get_pos().x + static_cast<float32>(direction) * 1.2f * this->speed_ * delta_time, floor(this->get_pos().y));
	this->set_scale(glm::vec2(static_cast<float32>(-direction), 1.0f));

	this->anim_time_ -= delta_time;

	return true;
}

auto Player::dead(const float32& delta_time) -> bool
{
	this->anim_time_ = 0.5f * static_cast<float32>(anim_time_ <= 0.0f) + anim_time_;
	this->state_ = Player::State::DEAD;

	this->temp_ = this->sprites_["dead"][0];
	this->anim_time_ -= delta_time;

	if (anim_time_ <= 0.0f)
	{
		this->set_pos(static_cast<float32>(this->screen_width_) / 2.0f, floor(this->get_pos().y));
		this->invis_ = true;
		this->invis_time_ = 10.0f;
	}

	return true;
}

auto Player::get_state() -> State
{
	return static_cast<State>(this->state_);
}

auto Player::get_invis() -> bool
{
	return this->invis_;
}

void Player::set_state(const State& state)
{
	this->new_state_ = state;
}

void Player::init_textures()
{
	this->sprites_["stand"] = std::vector<Sprite*>
	{
		new Sprite(this->texture_, glm::ivec4(5, 0, 28, 34),	glm::ivec2(screen_width_, screen_height_)) 
	};
	this->sprites_["run"] = std::vector<Sprite*>
	{
		new Sprite(this->texture_, glm::ivec4(66, 0, 95, 34),	 glm::ivec2(screen_width_, screen_height_)),
		new Sprite(this->texture_, glm::ivec4(98, 0, 127, 34), glm::ivec2(screen_width_, screen_height_)) 
	};
	this->sprites_["attack1"] = std::vector<Sprite*>
	{
		new Sprite(this->texture_, glm::ivec4(4, 86, 46, 128),	glm::ivec2(screen_width_, screen_height_)),
		new Sprite(this->texture_, glm::ivec4(48, 86, 90, 128),	glm::ivec2(screen_width_, screen_height_)),
		new Sprite(this->texture_, glm::ivec4(90, 86, 132, 128),	glm::ivec2(screen_width_, screen_height_)),
		new Sprite(this->texture_, glm::ivec4(134, 86, 176, 128),	glm::ivec2(screen_width_, screen_height_))
	};
	this->sprites_["slide"] = std::vector<Sprite*>
	{
		new Sprite(this->texture_, glm::ivec4(162, 0, 209, 34), glm::ivec2(screen_width_, screen_height_)),
		new Sprite(this->texture_, glm::ivec4(212, 0, 259, 34), glm::ivec2(screen_width_, screen_height_))
	};
	this->sprites_["dead"] = std::vector<Sprite*>
	{
		new Sprite(this->texture_, glm::ivec4(2, 212, 32, 242), glm::ivec2(screen_width_, screen_height_))
	};
}

void Player::update_input(const float32& delta_time)
{
	if (this->new_state_ != this->state_ && this->anim_time_ <= 0.0f)
		this->state_ = this->new_state_;

	this->invis_ = this->invis_time_ > 0.0f;
	this->invis_time_ -= delta_time * static_cast<float32>(this->invis_time_ >= 0.0f);

	this->invis_flag_ = !this->invis_flag_ && this->invis_time_ > 0.0f;

	switch (this->state_)
	{
	case Player::State::DEAD:
		this->dead(delta_time);
		break;

	case Player::State::STAND:
		this->stand();
		break;

	case Player::State::RUN_R:
		this->run(delta_time, 1);
		break;

	case Player::State::RUN_L:
		this->run(delta_time, -1);
		break;

	case Player::State::ATTACK1:
		this->attack1(delta_time);
		break;

	case Player::State::SLIDE_R:
		this->slide(delta_time, 1);
		break;

	case Player::State::SLIDE_L:
		this->slide(delta_time, -1);
		break;	

	default:
		break;
	}
}

void Player::update(const float32& delta_time)
{
	if (this->get_pos().x < this->get_size().x + 1.0f)
		this->set_pos(floor(this->get_size().x + 1.0f), this->get_pos().y);
	if (this->get_pos().x > static_cast<float32>(this->screen_width_) - this->get_size().x)
		this->set_pos(floor(static_cast<float32>(this->screen_width_) - this->get_size().x), this->get_pos().y);

	this->update_input(delta_time);
}

void Player::draw(Shader* shader)
{
	if (!this->invis_flag_)
		this->temp_->draw(shader, this->pos_, this->scale_);
}
