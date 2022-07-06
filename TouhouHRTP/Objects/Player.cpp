#include "Player.h"


Player::Player(const uint32& screen_width, const uint32& screen_height)
{
	this->screen_width_ = screen_width;
	this->screen_height_ = screen_height;
	this->Player::init_textures();
	this->temp_ = this->sprites_["stand"][0];
}

Player::~Player()
{
	for (auto& kv : this->sprites_)
		for (auto sprite : kv.second)
			delete sprite;
	for (auto& kv : this->effects_)
		delete kv.second;
}

auto Player::stand	() -> bool
{
	this->state_ = Player::State::STAND;
	this->temp_ = this->sprites_["stand"][0];
	this->set_pos(this->get_pos().x, this->get_size().y);
	this->set_scale(glm::vec2(1.0f, 1.0f));
	this->anim_time_ = 0.0f;
	return true;
}

auto Player::run	(const float32& delta_time, const int8& direction) -> bool
{
	this->state_ = Player::State::RUN_L * (direction < 0) + Player::State::RUN_R * (direction > 0);
	this->iter_ %= this->sprites_["run"].size();

	this->temp_ = this->sprites_["run"][iter_];
	this->time_ += delta_time;
	
	this->time_ *= static_cast<float32>(this->time_ < 0.1f);
	this->iter_ += this->time_ == 0.0f;

	this->set_pos(this->get_pos().x 
		+ static_cast<float32>(direction) 
		* delta_time
		* this->speed_, this->get_size().y);
	this->set_scale(glm::vec2(static_cast<float32>(-direction), 1.0f));

	this->anim_time_ = 0.0f;

	return true;
}

auto Player::attack1(const float32& delta_time) -> bool
{
	constexpr float32 speed_time = 0.5f;
		
	this->state_ = Player::State::ATTACK1;

	if (this->iter_ == this->sprites_["attack1"].size())
		this->flag_ = !this->flag_;

	this->iter_ %= this->sprites_["attack1"].size();

	this->temp_ = this->sprites_["attack1"][this->iter_];
	this->time_ += delta_time;
	
	this->time_ *= static_cast<float32>(this->time_ < speed_time / static_cast<float32>(2 * this->sprites_["attack1"].size()));
	this->iter_ += this->time_ == 0.0f;

	this->set_pos(this->get_pos().x, this->get_size().y);
	this->set_scale(glm::vec2(static_cast<float32>(-!this->flag_ + this->flag_), 1.0f));

	this->anim_time_ += delta_time;
	if (this->anim_time_ >= speed_time)
		this->anim_time_ = 0.0f;

	return true;
}

auto Player::attack2(const float32& delta_time, const int8& direction, const bool& no_stop) -> bool
{
	constexpr float32 speed_time = 0.8f;
	this->state_ = !no_stop * (Player::State::ATTACK2_L * (direction < 0) + Player::State::ATTACK2_R * (direction > 0))
		+ no_stop * (Player::State::SLIDE_L_A2 * (direction < 0) + Player::State::SLIDE_R_A2 * (direction > 0));

	this->iter_ %= this->sprites_.at("attack2").size();

	this->temp_ = this->sprites_.at("attack2")[this->iter_];
	this->time_ += delta_time;

	this->time_ *= static_cast<float32>(this->time_ < speed_time / static_cast<float32>(this->sprites_["attack2"].size()));
	this->iter_ += this->time_ == 0.0f;
	
	this->set_pos(this->get_pos().x 
		+ static_cast<float32>(no_stop)
		* static_cast<float32>(direction) 
		* delta_time 
		* 0.4f * this->speed_, this->get_size().y);
	this->set_scale(glm::vec2(static_cast<float32>(-direction), 1.0f));

	this->anim_time_ += delta_time;
	if (anim_time_ >= speed_time)
	{
		this->anim_time_ = 0.0f;
		//this->new_state_ = Player::State::STAND;
		this->new_state_ = (State::RUN_L * (direction < 0) + State::RUN_R * (direction > 0));
	}

	return true;
}

auto Player::attack3(const float32& delta_time, const int8& direction) -> bool
{
	this->state_ = Player::State::SLIDE_L_A3 * (direction < 0) + Player::State::SLIDE_R_A3 * (direction > 0);

	this->temp_ = this->sprites_["attack3"][0];

	this->set_pos(this->get_pos().x
		+ static_cast<float32>(direction) 
		* delta_time 
		* 1.4f * this->speed_, this->get_size().y);
	this->set_scale(glm::vec2(static_cast<float32>(-direction), 1.0f));

	this->anim_time_ += delta_time;
	if (anim_time_ >= 0.4f)
	{
		this->anim_time_ = 0.0f;
		//this->new_state_ = Player::State::STAND;
		this->new_state_ = (State::RUN_L * (direction < 0) + State::RUN_R * (direction > 0));
	}
	return true;
}

auto Player::slide	(const float32& delta_time, const int8& direction) -> bool
{
	this->state_ = Player::State::SLIDE_L * (direction < 0) + Player::State::SLIDE_R * (direction > 0);
	
	this->iter_ %= this->sprites_["slide"].size();

	this->temp_ = this->sprites_["slide"][iter_];
	this->time_ += delta_time;

	this->time_ *= static_cast<float32>(this->time_ < 0.1f);
	this->iter_ += this->time_ == 0.0f;

	this->set_pos(this->get_pos().x 
		+ static_cast<float32>(direction) 
		* delta_time 
		* 1.2f * this->speed_, this->get_size().y);
	this->set_scale(glm::vec2(static_cast<float32>(-direction), 1.0f));

	this->anim_time_ += delta_time;

	if (anim_time_ >= 0.22f)
	{
		this->anim_time_ = 0.0f;
		this->new_state_ = (Player::State::RUN_L * (direction < 0) + Player::State::RUN_R * (direction > 0));
	}
	return true;
}

auto Player::dead(const float32& delta_time) -> bool
{
	constexpr float32 time = 1.5f;
	constexpr float32 k = 350.0f;
	if (this->anim_time_ >= time)
		this->anim_time_ = 0.0f;
	this->state_ = Player::State::DEAD;

	this->temp_ = this->sprites_["dead"][0];
	this->anim_time_ += delta_time;
	
	this->effects_.at("death1")->set_pos(
		-k * powf(this->anim_time_, 2) 
		+ this->anim_time_ * (static_cast<float32>(this->screen_width_) / 2.0f - this->get_pos().x) / time
		+ this->anim_time_ * k * time
		+ this->get_pos().x, floor(this->get_pos().y));
	this->effects_.at("death2")->set_pos(
		k * powf(this->anim_time_, 2) 
		+ this->anim_time_ * (static_cast<float32>(this->screen_width_) / 2.0f - this->get_pos().x) / time
		- this->anim_time_ * k * time
		+ this->get_pos().x, floor(this->get_pos().y));

	if (anim_time_ >= time)
	{
		this->set_pos(static_cast<float32>(this->screen_width_) / 2.0f, this->get_size().y);
		this->invis_ = true;
		this->invis_time_ = 10.0f;
		this->anim_time_ = 0.0f;
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

void Player::set_old_state(const State& state)
{
	this->state_ = state;
}

void Player::init_textures()
{
	this->sprites_["stand"] = std::vector<Sprite*>
	{
		new Sprite(Resources::get_texture("reimu"), glm::ivec4(7, 0, 28, 34),		glm::ivec2(screen_width_, screen_height_)) 
	};
	this->sprites_["run"] = std::vector<Sprite*>
	{
		new Sprite(Resources::get_texture("reimu"), glm::ivec4(66, 0, 96, 34),		glm::ivec2(screen_width_, screen_height_)),
		new Sprite(Resources::get_texture("reimu"), glm::ivec4(98, 0, 128, 34),		glm::ivec2(screen_width_, screen_height_)) 
	};
	this->sprites_["attack1"] = std::vector<Sprite*>
	{
		new Sprite(Resources::get_texture("reimu"), glm::ivec4(4, 86, 46, 128),		glm::ivec2(screen_width_, screen_height_)),
		new Sprite(Resources::get_texture("reimu"), glm::ivec4(48, 86, 90, 128),	glm::ivec2(screen_width_, screen_height_)),
		new Sprite(Resources::get_texture("reimu"), glm::ivec4(90, 86, 132, 128),	glm::ivec2(screen_width_, screen_height_)),
		new Sprite(Resources::get_texture("reimu"), glm::ivec4(134, 86, 176, 128),	glm::ivec2(screen_width_, screen_height_))
	};
	this->sprites_["attack2"] = std::vector<Sprite*>
	{
		new Sprite(Resources::get_texture("reimu"), glm::ivec4(1, 36, 49, 84),		glm::ivec2(screen_width_, screen_height_)),
		new Sprite(Resources::get_texture("reimu"), glm::ivec4(52, 36, 100, 84),	glm::ivec2(screen_width_, screen_height_)),
		new Sprite(Resources::get_texture("reimu"), glm::ivec4(103, 36, 151, 84),	glm::ivec2(screen_width_, screen_height_)),
		new Sprite(Resources::get_texture("reimu"), glm::ivec4(152, 36, 200, 84),	glm::ivec2(screen_width_, screen_height_)),
		new Sprite(Resources::get_texture("reimu"), glm::ivec4(203, 36, 251, 84),	glm::ivec2(screen_width_, screen_height_)),
		new Sprite(Resources::get_texture("reimu"), glm::ivec4(254, 36, 302, 84),	glm::ivec2(screen_width_, screen_height_))
	};
	this->sprites_["attack3"] = std::vector<Sprite*>
	{
		new Sprite(Resources::get_texture("reimu"), glm::ivec4(262, 0, 310, 33),	glm::ivec2(screen_width_, screen_height_))
	};
	this->sprites_["slide"] = std::vector<Sprite*>
	{
		new Sprite(Resources::get_texture("reimu"), glm::ivec4(162, 0, 210, 33),	glm::ivec2(screen_width_, screen_height_)),
		new Sprite(Resources::get_texture("reimu"), glm::ivec4(212, 0, 260, 33),	glm::ivec2(screen_width_, screen_height_))
	};
	this->sprites_["dead"] = std::vector<Sprite*>
	{
		new Sprite(Resources::get_texture("reimu"), glm::ivec4(2, 212, 32, 242),	glm::ivec2(screen_width_, screen_height_))
	};

	this->effects_["death1"]	= new Rectangle(glm::ivec2(40, 40), glm::ivec2(this->screen_width_, this->screen_height_));
	this->effects_["death2"]	= new Rectangle(glm::ivec2(40, 40), glm::ivec2(this->screen_width_, this->screen_height_));
	this->effects_["invis"]	= new Rectangle(glm::ivec2(40, 40), glm::ivec2(this->screen_width_, this->screen_height_));
}

void Player::update_input(const float32& delta_time)
{
	if (this->new_state_ != this->state_ && this->anim_time_ <= 0.0f)
		this->state_ = this->new_state_;

	this->invis_ = this->invis_time_ > 0.0f;
	this->invis_time_ -= delta_time * static_cast<float32>(this->invis_time_ >= 0.0f);
	
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

	case Player::State::ATTACK2_R:
		this->attack2(delta_time, 1, false);
		break;

	case Player::State::ATTACK2_L:
		this->attack2(delta_time, -1, false);
		break;

	case Player::State::SLIDE_R:
		this->slide(delta_time, 1);
		break;

	case Player::State::SLIDE_L:
		this->slide(delta_time, -1);
		break;

	case Player::State::SLIDE_R_A2:
		this->attack2(delta_time, 1, true);
		break;

	case Player::State::SLIDE_L_A2:
		this->attack2(delta_time, -1, true);
		break;

	case Player::State::SLIDE_R_A3:
		this->attack3(delta_time, 1);
		break;
		
	case Player::State::SLIDE_L_A3:
		this->attack3(delta_time, -1);
		break;

	default:
		break;
	}
}

void Player::update(const float32& delta_time)
{
	Resources::get_shader("sparkle")->use();
	Resources::get_shader("sparkle")->setFloat("time", static_cast<float32>(glfwGetTime()));

	if (this->get_pos().x < this->get_size().x + 1.0f)
		this->set_pos(floor(this->get_size().x + 1.0f), this->get_pos().y);
	if (this->get_pos().x > static_cast<float32>(this->screen_width_) - this->get_size().x)
		this->set_pos(floor(static_cast<float32>(this->screen_width_) - this->get_size().x), this->get_pos().y);
	if (this->invis_)
		this->effects_.at("invis")->set_pos(this->get_pos().x, this->get_pos().y - 2.0f);

	this->update_input(delta_time);
}

void Player::draw()
{
	this->temp_->draw(Resources::get_shader("sprite"), this->pos_, this->scale_);
	if (this->invis_)
	{
		Resources::get_shader("sparkle")->use();
		Resources::get_shader("sparkle")->setVec3("uCol", glm::vec3(1.0f, 1.0f, 0.0f));
		Resources::get_shader("sparkle")->setFloat("range", 6.0f);
		this->effects_.at("invis")->draw(Resources::get_shader("sparkle"));
	}

	if (this->state_ == Player::State::DEAD)
	{
		Resources::get_shader("sparkle")->use();
		Resources::get_shader("sparkle")->setVec3("uCol", glm::vec3(1.0f, 0.0f, 0.0f));
		Resources::get_shader("sparkle")->setFloat("range", 2.0f);
		this->effects_.at("death1")->draw(Resources::get_shader("sparkle"));
		this->effects_.at("death2")->draw(Resources::get_shader("sparkle"));
	}
}
