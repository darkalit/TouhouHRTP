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
	return true;
}

auto Player::run	(const float32& delta_time, const int8& direction) -> bool
{
	this->state_ = State::RUNNING;
	this->iter_ %= this->sprites_["run"].size();

	this->temp_ = this->sprites_["run"][iter_];
	this->time_++;

	this->time_ *= static_cast<float32>(this->time_ * delta_time < 0.1f);
	this->iter_ += static_cast<uint32>(this->time_) == 0;

	this->set_pos(this->get_pos().x + static_cast<float32>(direction) * 100.0f * delta_time, floor(this->get_pos().y));

	this->set_scale(glm::vec2(static_cast<float32>(-direction), 1.0f));

	// left
	//this->set_pos(this->get_pos().x - 0.4f, floor(this->get_pos().y));
	//if (this->get_pos().x < this->get_size().x + 1.0f)
	//	this->set_pos(floor(this->get_size().x + 1.0f), this->get_pos().y);
	//this->set_scale(glm::vec2(1.0f, 1.0f));

	// right
	//this->set_pos(floor(static_cast<float32>(direction > 0) * static_cast<float32>(this->screen_width_) + static_cast<float32>(direction) * this->get_size().x), this->get_pos().y);
	//if (this->get_pos().x > static_cast<float32>(this->screen_width_) + static_cast<float32>(direction) * this->get_size().x)
	//	this->set_pos(floor(static_cast<float32>(this->screen_width_) - this->get_size().x), this->get_pos().y);

	return true;
}

auto Player::slide	(const float32& delta_time, const int8& direction) -> bool
{
	this->state_ = State::SLIDING_L * (direction == -1) + State::SLIDING_R * (direction == 1);
	this->iter_ %= this->sprites_["slide"].size();

	this->temp_ = this->sprites_["slide"][iter_];
	this->time_++;

	this->time_ *= static_cast<float32>(this->time_ * delta_time < 0.1f);
	this->iter_ += static_cast<uint32>(this->time_) == 0;

	this->set_pos(this->get_pos().x + static_cast<float32>(direction) * 200.0f * delta_time, floor(this->get_pos().y));
	this->set_scale(glm::vec2(static_cast<float32>(-direction), 1.0f));

	return true;

	//// LEFT
	//if (!anim_flag_ls_)
	//	this->anim_time_ = 0.0f;

	//if (this->anim_time_ * delta_time < 0.5f)
	//{
	//	anim_flag_ls_ = true;
	//	this->set_pos(floor(this->get_pos().x - 1.0f), floor(this->get_pos().y));
	//}
	//else
	//	anim_flag_ls_ = false;
	//this->anim_time_++;

	//// RIGHT
	//if (!anim_flag_rs_)
	//	this->anim_time_ = 0.0f;

	//if (this->anim_time_ * delta_time < 0.5f)
	//{
	//	anim_flag_rs_ = true;
	//	this->set_pos(floor(this->get_pos().x + 1.0f), floor(this->get_pos().y));
	//}
	//else
	//	anim_flag_rs_ = false;
	//this->anim_time_++;

	//this->set_scale(glm::vec2(static_cast<float32>(-direction), 1.0f));
}

auto Player::get_state() -> State
{
	return static_cast<State>(this->state_);
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
	this->sprites_["slide"] = std::vector<Sprite*>
	{
		new Sprite(this->texture_, glm::ivec4(162, 0, 209, 34), glm::ivec2(screen_width_, screen_height_)),
		new Sprite(this->texture_, glm::ivec4(212, 0, 259, 34), glm::ivec2(screen_width_, screen_height_))
	};	
}

void Player::update_input(const float32& delta_time)//(GLFWwindow* window, int key, int scancode, int action, int mods)
{	
	//if (!(glfwGetKey(window_, GLFW_KEY_RIGHT) || glfwGetKey(window_, GLFW_KEY_LEFT)))
	//{
	//	this->stand();
	//	this->set_pos(this->get_pos().x, this->get_pos().y);
	//	this->set_scale(glm::vec2(1.0f, 1.0f));
	//}
	//else if (glfwGetKey(window_, GLFW_KEY_LEFT) && !anim_flag_ls_ && !anim_flag_rs_)
	//{
	//	this->run(delta_time);
	//	this->set_pos(this->get_pos().x - 0.4f, floor(this->get_pos().y));
	//	if (this->get_pos().x < this->get_size().x + 1.0f)
	//		this->set_pos(floor(this->get_size().x + 1.0f), this->get_pos().y);
	//	this->set_scale(glm::vec2(1.0f, 1.0f));
	//}
	//else if (glfwGetKey(window_, GLFW_KEY_RIGHT) && !anim_flag_rs_ && !anim_flag_ls_)
	//{
	//	this->run(delta_time);
	//	this->set_pos(this->get_pos().x + 0.4f, floor(this->get_pos().y));
	//	if (this->get_pos().x > (static_cast<float32>(this->screen_width_) - this->get_size().x))
	//		this->set_pos(floor(static_cast<float32>(this->screen_width_) - this->get_size().x), this->get_pos().y);
	//	this->set_scale(glm::vec2(-1.0f, 1.0f));
	//}
	//if (((glfwGetKey(window_, GLFW_KEY_X) && glfwGetKey(window_, GLFW_KEY_LEFT)) || anim_flag_ls_) && anim_flag_rs_ == false)
	//{
	//	this->slide(delta_time);
	//			
	//	if (anim_flag_ls_ == false)
	//		this->anim_time_ = 0.0f;

	//	if (this->anim_time_ * delta_time < 0.5f)
	//	{
	//		anim_flag_ls_ = true;
	//		this->set_pos(floor(this->get_pos().x - 1.0f), floor(this->get_pos().y));
	//	}
	//	else
	//		anim_flag_ls_ = false;
	//	this->anim_time_++;

	//	if (this->get_pos().x < this->get_size().x + 1.0f)
	//		this->set_pos(floor(this->get_size().x + 1.0f), this->get_pos().y);

	//	this->set_scale(glm::vec2(1.0f, 1.0f));
	//}
	//else if (((glfwGetKey(window_, GLFW_KEY_X) && glfwGetKey(window_, GLFW_KEY_RIGHT)) || anim_flag_rs_) && anim_flag_ls_ == false)
	//{
	//	this->slide(delta_time);
	//	if (!anim_flag_rs_)
	//		this->anim_time_ = 0.0f;

	//	if (this->anim_time_ * delta_time < 0.5f)
	//	{
	//		anim_flag_rs_ = true;
	//		this->set_pos(floor(this->get_pos().x + 1.0f), floor(this->get_pos().y));
	//	}
	//	else
	//		anim_flag_rs_ = false;
	//	this->anim_time_++;

	//	if (this->get_pos().x > (static_cast<float32>(this->screen_width_) - this->get_size().x))
	//		this->set_pos(floor(static_cast<float32>(this->screen_width_) - this->get_size().x), this->get_pos().y);

	//	this->set_scale(glm::vec2(-1.0f, 1.0f));
	//}	
}

void Player::update(const float32& delta_time)
{
	if (this->get_pos().x < this->get_size().x + 1.0f)
		this->set_pos(floor(this->get_size().x + 1.0f), this->get_pos().y);
	if (this->get_pos().x > static_cast<float32>(this->screen_width_) - this->get_size().x)
		this->set_pos(floor(static_cast<float32>(this->screen_width_) - this->get_size().x), this->get_pos().y);
}

void Player::draw()
{
	this->temp_->draw(this->pos_, this->scale_);
}
