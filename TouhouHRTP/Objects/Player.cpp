#include "Player.h"


Player::Player(const uint32& screen_width, const uint32& screen_height)
{
	this->screen_width_ = screen_width;
	this->screen_height_ = screen_height;
	this->effects_["death1"]	= new Rectangle(glm::ivec2(40, 40), glm::ivec2(this->screen_width_, this->screen_height_));
	this->effects_["death2"]	= new Rectangle(glm::ivec2(40, 40), glm::ivec2(this->screen_width_, this->screen_height_));
	this->effects_["invis"]		= new Rectangle(glm::ivec2(40, 40), glm::ivec2(this->screen_width_, this->screen_height_));
	this->temp_ = Resources::get_sprites("PStand")[0];
}

Player::~Player()
{
	for (auto& kv : this->effects_)
		delete kv.second;
}

void Player::stand()
{
	this->state_ = STAND;
	this->temp_ = Resources::get_sprites("PStand")[0];
	this->set_pos(this->get_pos().x, this->get_size().y / 2.0f);
	this->set_scale(glm::vec2(1.0f));
	this->anim_time_ = 0.0f;
}

void Player::run(const float32& delta_time, const int8& direction)
{
	this->state_ = RUN_L * (direction < 0) + RUN_R * (direction > 0);
	this->iter_ %= Resources::get_sprites("PRun").size();

	this->temp_ = Resources::get_sprites("PRun")[this->iter_];
	this->time_ += delta_time;
	
	this->time_ *= static_cast<float32>(this->time_ < 0.1f);
	this->iter_ += this->time_ == 0.0f;

	this->set_pos(this->get_pos().x 
		+ static_cast<float32>(direction) 
		* delta_time
		* this->speed_, this->get_size().y / 2.0f);
	this->set_scale(glm::vec2(static_cast<float32>(-direction), 1.0f));

	this->anim_time_ = 0.0f;
}

void Player::attack1(const float32& delta_time)
{
	constexpr float32 speed_time = 0.5f;
		
	this->state_ = ATTACK1;

	if (this->iter_ == Resources::get_sprites("PAttack1").size())
		this->flag_ = !this->flag_;

	this->iter_ %= Resources::get_sprites("PAttack1").size();

	this->temp_ = Resources::get_sprites("PAttack1")[this->iter_];
	this->time_ += delta_time;
	
	this->time_ *= static_cast<float32>(this->time_ < speed_time / static_cast<float32>(2 * Resources::get_sprites("PAttack1").size()));
	this->iter_ += this->time_ == 0.0f;

	this->set_pos(this->get_pos().x, this->get_size().y / 2.0f);
	this->set_scale(glm::vec2(static_cast<float32>(-!this->flag_ + this->flag_), 1.0f));

	this->anim_time_ += delta_time;
	if (this->anim_time_ >= speed_time)
		this->anim_time_ = 0.0f;
}

void Player::attack2(const float32& delta_time, const int8& direction, const bool& no_stop)
{
	constexpr float32 speed_time = 0.8f;
	this->state_ = !no_stop * (ATTACK2_L * (direction < 0) + ATTACK2_R * (direction > 0))
		+ no_stop * (SLIDE_L_A2 * (direction < 0) + SLIDE_R_A2 * (direction > 0));

	this->iter_ %= Resources::get_sprites("PAttack2").size();

	this->temp_ = Resources::get_sprites("PAttack2")[this->iter_];
	this->time_ += delta_time;

	this->time_ *= static_cast<float32>(this->time_ < speed_time / static_cast<float32>(Resources::get_sprites("PAttack2").size()));
	this->iter_ += this->time_ == 0.0f;
	
	this->set_pos(this->get_pos().x 
		+ static_cast<float32>(no_stop)
		* static_cast<float32>(direction) 
		* delta_time 
		* 0.4f * this->speed_, this->get_size().y / 2.0f);
	this->set_scale(glm::vec2(static_cast<float32>(-direction), 1.0f));

	this->anim_time_ += delta_time;
	if (anim_time_ >= speed_time)
	{
		this->anim_time_ = 0.0f;
		//this->new_state_ = Player::State::STAND;
		this->new_state_ = (RUN_L * (direction < 0) + RUN_R * (direction > 0));
	}
}

void Player::attack3(const float32& delta_time, const int8& direction)
{
	this->state_ = SLIDE_L_A3 * (direction < 0) + SLIDE_R_A3 * (direction > 0);

	this->temp_ = Resources::get_sprites("PAttack3")[0];

	this->set_pos(this->get_pos().x
		+ static_cast<float32>(direction) 
		* delta_time 
		* 1.4f * this->speed_, this->get_size().y / 2.0f);
	this->set_scale(glm::vec2(static_cast<float32>(-direction), 1.0f));

	this->anim_time_ += delta_time;
	if (anim_time_ >= 0.4f)
	{
		this->anim_time_ = 0.0f;
		this->new_state_ = (State::RUN_L * (direction < 0) + State::RUN_R * (direction > 0));
	}
}

void Player::bomb(const float32& delta_time)
{
	this->state_ = BOMB;

	this->iter_ %= Resources::get_sprites("boom").size();
	this->temp_ = Resources::get_sprites("PStand")[0];
	this->temp_boom_ = Resources::get_sprites("boom")[this->iter_];
	this->time_ += delta_time;

	this->time_ *= static_cast<float32>(this->time_ < 0.2f);
	this->iter_ += this->time_ == 0.0f;

	this->anim_time_ += delta_time;

	Resources::get_shader("screen")->use();
	Resources::get_shader("screen")->setBool("shake", true);
	Resources::get_shader("screen")->setFloat("time", static_cast<float32>(glfwGetTime()));

	this->invis_ = true;
	if (anim_time_ >= 1.22f)
	{
		this->anim_time_ = 0.0f;
		this->new_state_ = STAND;
		this->bombs--;
		invis_time_ = 0.5f;
		Resources::get_shader("screen")->use();
		Resources::get_shader("screen")->setBool("shake", false);
	}
}

void Player::slide	(const float32& delta_time, const int8& direction)
{
	this->state_ = SLIDE_L * (direction < 0) + SLIDE_R * (direction > 0);
	
	this->iter_ %= Resources::get_sprites("PSlide").size();

	this->temp_ = Resources::get_sprites("PSlide")[this->iter_];
	this->time_ += delta_time;

	this->time_ *= static_cast<float32>(this->time_ < 0.1f);
	this->iter_ += this->time_ == 0.0f;

	this->set_pos(this->get_pos().x 
		+ static_cast<float32>(direction) 
		* delta_time 
		* 1.2f * this->speed_, this->get_size().y / 2.0f);
	this->set_scale(glm::vec2(static_cast<float32>(-direction), 1.0f));

	this->anim_time_ += delta_time;

	if (anim_time_ >= 0.22f)
	{
		this->anim_time_ = 0.0f;
		this->new_state_ = (RUN_L * (direction < 0) + RUN_R * (direction > 0));
	}
}

void Player::dead(const float32& delta_time)
{
	constexpr float32 time = 1.5f;
	constexpr float32 k = 350.0f;
	if (this->anim_time_ >= time)
		this->anim_time_ = 0.0f;
	this->state_ = DEAD;

	this->temp_ = Resources::get_sprites("PDead")[0];
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

	Resources::get_shader("screen")->use();
	Resources::get_shader("screen")->setBool("shake", true);
	Resources::get_shader("screen")->setFloat("time", static_cast<float32>(glfwGetTime()));

	if (anim_time_ >= time)
	{
		this->set_pos(static_cast<float32>(this->screen_width_) / 2.0f, this->get_size().y / 2.0f);
		this->invis_ = true;
		this->invis_time_ = 2.0f;
		this->anim_time_ = 0.0f;
		this->hp -= this->hp > 0;
		this->bombs += this->hp > 0;
		Resources::get_shader("screen")->use();
		Resources::get_shader("screen")->setBool("shake", false);
	}
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

void Player::set_speed(const float32& speed)
{
	this->speed_ = speed;
}

void Player::reset()
{
	this->flag_ = true;
	this->invis_ = false;
	this->hp = 4;
	this->bombs = 1;
	this->rf_time_ = 0.0f;
	this->anim_time_ = 0.0f;
	this->invis_time_ = 0.0f;
	this->state_ = STAND;
	this->new_state_ = STAND;
	this->iter_ = 0.0f;
	this->time_ = 0.0f;
}

void Player::update_input(const float32& delta_time)
{
	if (this->new_state_ != this->state_ && this->anim_time_ <= 0.0f)
		this->state_ = this->new_state_;

	this->invis_ = this->invis_time_ > 0.0f;
	this->invis_time_ -= delta_time * static_cast<float32>(this->invis_time_ >= 0.0f);
	
	switch (this->state_)
	{
	case DEAD:
		this->dead(delta_time);
		break;

	case STAND:
		this->stand();
		break;

	case RUN_R:
		this->run(delta_time, 1);
		break;

	case RUN_L:
		this->run(delta_time, -1);
		break;

	case ATTACK1:
		this->attack1(delta_time);
		break;

	case ATTACK2_R:
		this->attack2(delta_time, 1, false);
		break;

	case ATTACK2_L:
		this->attack2(delta_time, -1, false);
		break;

	case SLIDE_R:
		this->slide(delta_time, 1);
		break;

	case SLIDE_L:
		this->slide(delta_time, -1);
		break;

	case SLIDE_R_A2:
		this->attack2(delta_time, 1, true);
		break;

	case SLIDE_L_A2:
		this->attack2(delta_time, -1, true);
		break;

	case SLIDE_R_A3:
		this->attack3(delta_time, 1);
		break;
		
	case SLIDE_L_A3:
		this->attack3(delta_time, -1);
		break;

	case BOMB:
		this->bomb(delta_time);
		break;

	default:
		break;
	}
}

void Player::update(const float32& delta_time)
{
	Resources::get_shader("sparkle")->use();
	Resources::get_shader("sparkle")->setFloat("time", static_cast<float32>(glfwGetTime()));

	if (this->get_pos().x < this->get_size().x / 2.0f + 1.0f)
		this->set_pos(floor(this->get_size().x / 2.0f + 1.0f), this->get_pos().y);
	if (this->get_pos().x > static_cast<float32>(this->screen_width_) - this->get_size().x / 2.0f)
		this->set_pos(floor(static_cast<float32>(this->screen_width_) - this->get_size().x / 2.0f), this->get_pos().y);
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

	if (this->state_ == DEAD)
	{
		Resources::get_shader("sparkle")->use();
		Resources::get_shader("sparkle")->setVec3("uCol", glm::vec3(1.0f, 0.0f, 0.0f));
		Resources::get_shader("sparkle")->setFloat("range", 2.0f);
		this->effects_.at("death1")->draw(Resources::get_shader("sparkle"));
		this->effects_.at("death2")->draw(Resources::get_shader("sparkle"));
	}
	if (this->state_ == BOMB)
	{
		this->temp_boom_->draw(Resources::get_shader("sprite"), glm::vec2(0.0f));
	}
}
